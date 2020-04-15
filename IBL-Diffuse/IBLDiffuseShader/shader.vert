#version 460

layout (location = 0) in vec2 position;

uniform mat4 Rotation;

out vec3 direction;

void main()
{
  direction = (Rotation * vec4(position.x, position.y, -1.0, 0.0)).xyz;
  gl_Position = vec4(position, 0.0, 1.0);
}
