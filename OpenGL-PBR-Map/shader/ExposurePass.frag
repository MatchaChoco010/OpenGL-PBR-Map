#version 460

in vec2 vUv;

layout (location = 0) out vec3 outputColor;

layout (binding = 0) uniform sampler2D inputTexture;

uniform float exposure;


const float HALF_MAX = 65504.0;

void main() {
  vec3 inputColor = texture(inputTexture, vUv).rgb;
  inputColor = clamp(inputColor, 0, HALF_MAX);

  vec3 exposuredColor = inputColor * exposure;

  outputColor = exposuredColor;
}
