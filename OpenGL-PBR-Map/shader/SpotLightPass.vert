#version 460

layout (location = 0) in vec4 position;

uniform mat4 ModelViewProjection;

void main()
{
  gl_Position = ModelViewProjection * position;
}
