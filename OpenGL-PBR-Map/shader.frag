#version 460

in vec3 vWorldPosition;
in vec3 vWorldNormal;

layout (location = 0) out vec4 fragment;

uniform vec3 worldCameraPosition;

const vec3 worldLightPosition = vec3(0.0, 5.0, 2.0);

const vec3 lightColor = vec3(1.0);

const vec3 Kdiff = vec3(0.8, 0.5, 0.2);
const vec3 Kspec = vec3(1.0);
const float shininess = 50;

void main() {
  vec3 L = normalize(worldLightPosition - vWorldPosition);
  vec3 V = normalize(worldCameraPosition - vWorldPosition);
  vec3 N = normalize(vWorldNormal);
  vec3 H = normalize(L + V);

  // Lambert
  vec3 diffuse = max(dot(L, N), 0) * Kdiff * lightColor;

  // Blinn-phong
  vec3 specular = pow(max(dot(N, H), 0), shininess) * Kspec * lightColor;

  vec3 color = diffuse + specular;

  fragment = vec4(color, 1.0);
}
