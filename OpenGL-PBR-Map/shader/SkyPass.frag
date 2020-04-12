#version 460

in vec3 direction;

layout (location = 0) out vec3 outputColor;

layout (binding = 0) uniform sampler2D SkyImage;

uniform float skyIntensity;

const float PI = 3.14159265358979323846;


// ACES ######################################################################
const mat3 sRGB_2_AP0 = mat3(
  0.4397010, 0.0897923, 0.0175440,
  0.3829780, 0.8134230, 0.1115440,
  0.1773350, 0.0967616, 0.8707040
);

vec3 sRGBToACES(vec3 srgb)
{
  return sRGB_2_AP0 * srgb;
}
// ###########################################################################


void main()
{
  vec3 dir = normalize(direction);

  vec2 texcoord = vec2(
    atan(dir.z, dir.x) / (2 * PI) + 0.5,
    atan(dir.y, length(dir.xz)) / PI + 0.5
  );

  // flip Y
  texcoord.y = 1 - texcoord.y;

  outputColor = sRGBToACES(texture(SkyImage, texcoord).rgb) * skyIntensity;
}
