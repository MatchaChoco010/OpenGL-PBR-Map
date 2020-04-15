#version 460

layout (location = 0) in vec2 position;

uniform mat4 CameraRotationMatrix;
uniform float yTan;
uniform float xTan;

out vec3 direction;

void main()
{
  direction = (CameraRotationMatrix * vec4(xTan * position.x, yTan * position.y, -1.0, 0.0)).xyz;
  gl_Position = vec4(position, 0.0, 1.0);
}
