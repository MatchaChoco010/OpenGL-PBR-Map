#version 460

layout (location = 0) in vec2 position;

uniform mat4 CameraRotationMatrix;
uniform float yTan;
uniform float xTan;

out vec3 direction;

void main()
{
  vec2 pos = position * 2 - vec2(1);
  direction = (CameraRotationMatrix * vec4(xTan * pos.x, yTan * pos.y, -1.0, 0.0)).xyz;
  gl_Position = vec4(pos, 0.0, 1.0);
}
