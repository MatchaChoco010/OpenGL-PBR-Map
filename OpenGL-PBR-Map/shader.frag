#version 460

in vec3 vWorldPosition;
in vec3 vWorldNormal;

layout (location = 0) out vec4 fragment;

const vec3 worldLightPosition = vec3(0.0, 5.0, 2.0);
const vec3 lightColor = vec3(1.0);
const vec3 Kdiff = vec3(0.8);

void main() {
  vec3 L = normalize(worldLightPosition - vWorldPosition);
  vec3 N = normalize(vWorldNormal);

  vec3 color = max(dot(L, N), 0) * Kdiff * lightColor;

  fragment = vec4(color, 1.0);
}
