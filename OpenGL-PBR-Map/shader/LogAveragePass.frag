#version 460

in vec2 vUv;

layout (location = 0) out float outputColor;

uniform sampler2D inputTexture;


const float HALF_MAX = 65504.0;


float luminance(vec3 rgb)
{
  return 0.298912 * rgb.r + 0.586611 * rgb.g + 0.114478 * rgb.b;
}

void main()
{
  vec3 inputColor = texture(inputTexture, vUv).rgb;
  inputColor = clamp(inputColor, 0, HALF_MAX);

  float l = luminance(inputColor);
  const float EPSILON = 0.01;
  outputColor = log(l + EPSILON);
}
