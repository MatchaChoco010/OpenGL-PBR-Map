#version 460

layout (location = 0) out vec3 outRadiance;

layout (binding = 0) uniform sampler2D GBuffer0;
layout (binding = 1) uniform sampler2D GBuffer1;
layout (binding = 2) uniform sampler2D GBuffer2;
layout (binding = 3) uniform sampler2DShadow ShadowMap;

uniform vec3 worldLightPosition;
uniform float lightIntensity; // lm
uniform vec3 lightColor;
uniform float lightRange;
uniform vec3 lightDirection;
uniform float lightAngle; // radian
uniform float lightBlend; // 0-1

uniform vec3 worldCameraPos;
uniform mat4 ViewProjectionI;
uniform vec2 ProjectionParams; // x: near, y: far

uniform mat4 LightViewProjection;


const float PI = 3.14159265358979323846;


// Calc gbuffer texcoord #######################################################
vec2 CalcTexCoord()
{
   return gl_FragCoord.xy / textureSize(GBuffer0, 0);
}
// #############################################################################


// world pos from depth texture ################################################
float DecodeDepth(float d)
{
  return -d * (ProjectionParams.y - ProjectionParams.x) - ProjectionParams.x;
}

vec3 worldPosFromDepth(float d, vec2 uv)
{
  float depth = DecodeDepth(d);
  float m22 = -(ProjectionParams.y + ProjectionParams.x) / (ProjectionParams.y - ProjectionParams.x);
  float m23 = -2.0 * ProjectionParams.y * ProjectionParams.x / (ProjectionParams.y - ProjectionParams.x);
  float z = depth * m22 + m23;
  float w = -depth;
  vec4 projectedPos = vec4(uv.x * 2.0 - 1.0, uv.y * 2.0 - 1.0, z / w, 1.0);
  vec4 worldPos = ViewProjectionI * projectedPos;
  return worldPos.xyz / worldPos.w;
}
// #############################################################################


// attenuation #################################################################
float DistanceAttenuation(float distance)
{
  float EPSILON = 0.01;
  float att = 1.0 / (distance * distance + EPSILON);
  float smoothatt = 1 - pow(distance / lightRange, 4.0);
  smoothatt = max(smoothatt, 0.0);
  smoothatt =  smoothatt * smoothatt;
  return att * smoothatt;
}

float AngleAttenuation(vec3 L)
{
  float outerTheta = lightAngle / 2.0;
  float innerTheta = outerTheta * (1.0 - lightBlend);
  float cos_s = dot(-L, normalize(lightDirection));
  float cos_u = cos(outerTheta);
  float cos_p = cos(innerTheta);
  float t = (cos_s - cos_u) / (cos_p - cos_u);
  t = clamp(t, 0, 1);
  return t * t;
}

vec3 LightIrradiance(float intensity, vec3 color, vec3 L, vec3 N, float distance)
{
  return 1.0 / PI * intensity * color * max(0, dot(L, N)) * DistanceAttenuation(distance) * AngleAttenuation(L);
}
// #############################################################################


// 3x3 PCF Shadow ##############################################################
float getShadowAttenuation(vec3 worldPos)
{
  vec4 lightPos = LightViewProjection * vec4(worldPos, 1.0);
  vec2 uv = lightPos.xy / lightPos.w * vec2(0.5) + vec2(0.5);
  float depthFromWorldPos = (lightPos.z / lightPos.w) * 0.5 + 0.5;

  ivec2 shadowMapSize = textureSize(ShadowMap, 0);
  vec2 offset = 1.0 / shadowMapSize.xy;

  float shadow = 0.0;
  for (int i = -1; i <= 1; i++)
  {
    for (int j = -1; j <= 1; j++)
    {
      vec3 UVC = vec3(uv + offset * vec2(i, j), depthFromWorldPos + 0.00001);
      shadow += texture(ShadowMap, UVC).x;
    }
  }
  return shadow / 9.0;
}
// #############################################################################


// BRDF ######################################################################
vec3 NormalizedLambert(vec3 diffuseColor) {
  return diffuseColor / PI;
}

vec3 F_Schlick(vec3 F0, vec3 H, vec3 V) {
  return (F0 + (1.0 - F0) * pow(1.0 - max(dot(V, H), 0), 5.0));
}

float D_GGX(float a, float NoH) {
  float a2 = a * a;
  float NoH2 = NoH * NoH;
  float d = NoH2 * (a2 - 1.0) + 1.0;
  return a2 / (PI * d * d);
}

float G_SchlickGGX(float a, float NoV) {
  float EPSILON = 0.001;
  float k = a * a / 2 + EPSILON;
  float up = NoV;
  float down = NoV * (1 - k) + k;
  return up / down;
}

float G_Smith(float a, float NoV, float NoL) {
  float ggx0 = G_SchlickGGX(1, NoV);
  float ggx1 = G_SchlickGGX(1, NoL);
  return ggx0 * ggx1;
}

vec3 BRDF(vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 L, vec3 H) {
  float EPSILON = 0.001;
  vec3 F0 = mix(vec3(0.04), albedo, metallic);
  float NoH = max(dot(N, H), 0);
  float NoV = max(dot(N, V), 0);
  float NoL = max(dot(N, L), 0);
  float a = roughness * roughness;

  // specular
  vec3 F = F_Schlick(F0, H, V);
  float D = D_GGX(a, NoH);
  float G = G_Smith(a, NoV, NoL);
  vec3 up = F * D * G;
  float down = max(4.0 * NoV * NoL, EPSILON);
  vec3 specular = up / down;

  // diffuse
  vec3 diffuse = NormalizedLambert(albedo);

  vec3 kD = vec3(1.0) - F;
  kD *= 1.0 - metallic;
  return kD * diffuse + specular;
}
// ###########################################################################


// ACES ######################################################################
const mat3 sRGB_2_AP0 = mat3(
  0.4397010, 0.0897923, 0.0175440,
  0.3829780, 0.8134230, 0.1115440,
  0.1773350, 0.0967616, 0.8707040
);

vec3 sRGBToACES(vec3 srgb)
{
  return sRGB_2_AP0 * srgb;
}
// ###########################################################################


void main()
{
  vec2 uv = CalcTexCoord();

  vec4 gbuffer0 = texture(GBuffer0, uv);
  vec4 gbuffer1 = texture(GBuffer1, uv);
  vec4 gbuffer2 = texture(GBuffer2, uv);

  vec3 albedo = gbuffer0.rgb;
  float metallic = gbuffer0.a;
  vec3 emissive = gbuffer1.rgb;
  float depth = gbuffer1.a;
  vec3 normal = gbuffer2.rgb * 2.0 - 1.0;
  float roughness = gbuffer2.a;

  vec3 worldPos = worldPosFromDepth(depth, uv);

  float shadow = getShadowAttenuation(worldPos);

  vec3 V = normalize(worldCameraPos - worldPos);
  vec3 N = normalize(normal);
  vec3 L = normalize(worldLightPosition - worldPos);
  vec3 H = normalize(L + V);

  float distance = length(worldLightPosition - worldPos);
  vec3 irradiance = LightIrradiance(lightIntensity, sRGBToACES(lightColor), L, N, distance) * shadow;

  outRadiance = BRDF(albedo, metallic, roughness, N, V, L, H) * irradiance;
}
