#version 460

in vec2 vUv;

layout (location = 0) out vec3 outRadiance;

layout (binding = 0) uniform sampler2D GBuffer0;
layout (binding = 1) uniform sampler2D GBuffer1;
layout (binding = 2) uniform sampler2D GBuffer2;
layout (binding = 3) uniform samplerCube IrradianceMap;

uniform vec3 worldCameraPos;
uniform mat4 ViewProjectionI;
uniform vec2 ProjectionParams; // x: near, y: far


const float PI = 3.14159265358979323846;
const float HALF_MAX = 65504.0;


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

vec3 F_SchlickWithRoughness(vec3 F0, vec3 H, vec3 V, float roughness) {
  return (F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - max(dot(V, H), 0), 5.0));
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

  vec3 F0 = mix(vec3(0.04), albedo, metallic);
  vec3 kS = F_SchlickWithRoughness(F0, N, V, roughness);
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  vec3 irradiance = clamp(texture(IrradianceMap, N).rgb, 0, HALF_MAX);
  vec3 diffuse = kD * NormalizedLambert(albedo) * irradiance;

  outRadiance = diffuse;
}
