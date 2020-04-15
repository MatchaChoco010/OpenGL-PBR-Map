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


vec3 SampleColor(vec3 dir) {
  dir = normalize(dir);

  vec2 texcoord = vec2(
    atan(dir.z, dir.x) / (2 * PI) + 0.5,
    atan(dir.y, length(dir.xz)) / PI + 0.5
  );

  // flip Y
  texcoord.y = 1 - texcoord.y;

  return sRGBToACES(texture(SkyImage, texcoord).rgb) * skyIntensity;
}

void main() {
  vec3 normal = normalize(direction);

  vec3 irradiance = vec3(0.0);

  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = cross(up, normal);
  up = cross(normal, right);

  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 64; j++) {
      float phi = 2 * PI * float(i) / 256.0;
      float theta = 0.5 * PI * float(j) / 64.0;

      vec3 tangentSampleVec = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
      vec3 sampleVec = tangentSampleVec.x * right + tangentSampleVec.y * normal + tangentSampleVec.z * up;

      irradiance += SampleColor(sampleVec) * cos(theta) * sin(theta);
    }
  }
  irradiance = PI * PI * irradiance / float(256 * 64);

  outputColor = irradiance;
}
