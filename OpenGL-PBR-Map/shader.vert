#version 460

layout (location = 0) in vec4 position;

uniform mat4 Model;
uniform mat4 ViewProjection;

void main() {
  gl_Position = ViewProjection * Model * position;
}
