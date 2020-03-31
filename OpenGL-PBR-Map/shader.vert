#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out vec3 vWorldPosition;
out vec3 vWorldNormal;

uniform mat4 Model;
uniform mat4 ModelIT;
uniform mat4 ViewProjection;

void main() {
  vec4 worldPosition = Model * position;
  gl_Position = ViewProjection * worldPosition;
  vWorldPosition = worldPosition.xyz / worldPosition.w;
  vWorldNormal = mat3(ModelIT) * normal;
}
