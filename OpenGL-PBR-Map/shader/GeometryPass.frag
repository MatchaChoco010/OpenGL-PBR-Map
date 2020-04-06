#version 460

in vec3 vWorldNormal;
in vec3 vWorldTangent;
in float vDepth;
in vec2 vUv;

layout (location = 0) out vec4 GBuffer0; // rgb: albedo, a: metallic
layout (location = 1) out vec4 GBuffer1; // rgb: emissive, a: depth
layout (location = 2) out vec4 GBuffer2; // rgb: world normal, a: roughness

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D metallicMap;
layout (binding = 2) uniform sampler2D roughnessMap;
layout (binding = 3) uniform sampler2D normalMap;
layout (binding = 4) uniform sampler2D emissiveMap;

uniform float emissiveIntensity;


const float PI = 3.14159265358979323846;

const mat3 sRGB_2_AP0 = mat3(
  0.4397010, 0.0897923, 0.0175440,
  0.3829780, 0.8134230, 0.1115440,
  0.1773350, 0.0967616, 0.8707040
);


vec3 sRGBToACES(vec3 srgb)
{
  return sRGB_2_AP0 * srgb;
}

vec3 GetNormal() {
  vec3 normal = normalize(vWorldNormal);
  vec3 bitangent = normalize(cross(normal, normalize(vWorldTangent)));
  vec3 tangent = normalize(cross(bitangent, normal));
  mat3 TBN = mat3(tangent, bitangent, normal);
  vec3 normalFromMap = texture(normalMap, vUv).rgb * 2 - 1;
  return normalize(TBN * normalFromMap);
}

void main()
{
  vec3 albedo = sRGBToACES(texture(albedoMap, vUv).rgb);
  float metallic = texture(metallicMap, vUv).r;
  float roughness = texture(roughnessMap, vUv).r;
  vec3 normal = GetNormal();
  vec3 emissive = sRGBToACES(texture(emissiveMap, vUv).rgb) * emissiveIntensity;

  GBuffer0 = vec4(albedo, metallic);
  GBuffer1 = vec4(emissive, vDepth);
  GBuffer2 = vec4(normal * 0.5 + 0.5, roughness);
}
