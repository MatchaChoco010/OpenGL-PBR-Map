#version 460

in vec2 vUv;

layout (location = 0) out vec3 outRadiance;

layout (binding = 0) uniform sampler2D GBuffer0;
layout (binding = 1) uniform sampler2D GBuffer1;
layout (binding = 2) uniform sampler2D GBuffer2;

uniform vec3 lightDirection;
uniform float lightIntensity; // lx
uniform vec3 lightColor;

uniform vec3 worldCameraPos;
uniform mat4 ViewProjectionI;
uniform vec2 ProjectionParams; // x: near, y: far


const float PI = 3.14159265358979323846;


// world pos from depth texture ##############################################
float DecodeDepth(float d)
{
  return -d * (ProjectionParams.y - ProjectionParams.x) - ProjectionParams.x;
}

vec3 worldPosFromDepth(float d)
{
  float depth = DecodeDepth(d);
  float m22 = -(ProjectionParams.y + ProjectionParams.x) / (ProjectionParams.y - ProjectionParams.x);
  float m23 = -2.0 * ProjectionParams.y * ProjectionParams.x / (ProjectionParams.y - ProjectionParams.x);
  float z = depth * m22 + m23;
  float w = -depth;
  vec4 projectedPos = vec4(vUv.x * 2.0 - 1.0, vUv.y * 2.0 - 1.0, z / w, 1.0);
  vec4 worldPos = ViewProjectionI * projectedPos;
  return worldPos.xyz / worldPos.w;
}
// ###########################################################################


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
  vec4 gbuffer0 = texture(GBuffer0, vUv);
  vec4 gbuffer1 = texture(GBuffer1, vUv);
  vec4 gbuffer2 = texture(GBuffer2, vUv);

  vec3 albedo = gbuffer0.rgb;
  float metallic = gbuffer0.a;
  vec3 emissive = gbuffer1.rgb;
  float depth = gbuffer1.a;
  vec3 normal = gbuffer2.rgb * 2.0 - 1.0;
  float roughness = gbuffer2.a;

  vec3 worldPos = worldPosFromDepth(depth);

  vec3 V = normalize(worldCameraPos - worldPos);
  vec3 N = normalize(normal);
  vec3 L = normalize(-lightDirection);
  vec3 H = normalize(L + V);

  vec3 irradiance = lightIntensity * sRGBToACES(lightColor) * dot(N, L);
  vec3 radianceDirectionalLight = BRDF(albedo, metallic, roughness, N, V, L, H) * irradiance;

  outRadiance = emissive + radianceDirectionalLight;
}
