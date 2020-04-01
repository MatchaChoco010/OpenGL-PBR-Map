#version 460

in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec2 vUv;

layout (location = 0) out vec4 fragment;

uniform vec3 worldCameraPosition;

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;

const vec3 worldLightPosition = vec3(0.0, 5.0, 2.0);

const vec3 lightColor = vec3(1.0);

const vec3 Kspec = vec3(1.0);
const float shininess = 50;

vec3 GetNormal() {
  vec3 normal = normalize(vWorldNormal);
  vec3 bitangent = normalize(cross(normal, normalize(vWorldTangent)));
  vec3 tangent = normalize(cross(bitangent, normal));
  mat3 TBN = mat3(tangent, bitangent, normal);
  vec3 normalFromMap = texture(normalMap, vUv).rgb * 2 - 1;
  return normalize(TBN * normalFromMap);
}

void main() {
  vec3 Kdiff = texture(albedoMap, vUv).rgb;

  vec3 L = normalize(worldLightPosition - vWorldPosition);
  vec3 V = normalize(worldCameraPosition - vWorldPosition);
  vec3 N = GetNormal();
  vec3 H = normalize(L + V);

  // Lambert
  vec3 diffuse = max(dot(L, N), 0) * Kdiff * lightColor;

  // Blinn-phong
  vec3 specular = pow(max(dot(N, H), 0), shininess) * Kspec * lightColor;

  vec3 color = diffuse + specular;

  fragment = vec4(color, 1.0);
}
