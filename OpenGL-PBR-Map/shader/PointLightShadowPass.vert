#version 460

layout (location = 0) in vec4 position;

uniform mat4 Model;

void main()
{
  gl_Position = Model * position;
}
