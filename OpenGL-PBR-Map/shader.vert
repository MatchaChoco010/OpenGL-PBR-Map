#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 vertexColor;

out vec3 color;

void main() {
  color = vertexColor;
  gl_Position = position;
}
