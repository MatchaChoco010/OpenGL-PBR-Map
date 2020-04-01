#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

out vec3 vWorldPosition;
out vec3 vWorldNormal;
out vec3 vWorldTangent;
out vec2 vUv;

uniform mat4 Model;
uniform mat4 ModelIT;
uniform mat4 ViewProjection;

void main() {
  vec4 worldPosition = Model * position;
  gl_Position = ViewProjection * worldPosition;
  vWorldPosition = worldPosition.xyz / worldPosition.w;
  vWorldNormal = mat3(ModelIT) * normal;
  vWorldTangent = mat3(ModelIT) * tangent;
  vUv = uv;
}
