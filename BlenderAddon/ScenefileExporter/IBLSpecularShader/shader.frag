#version 460

in vec3 direction;

layout (location = 0) out vec4 outputColor;

layout (binding = 0) uniform sampler2D SkyImage;

uniform float skyIntensity;
uniform float Roughness;

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


// Sample Color ##############################################################
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
// ###########################################################################


// Importance Sampling GGX ###################################################
vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N) {
  float a = Roughness * Roughness;

  float Phi = 2.0 * PI * Xi.x;
  float CosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
  float SinTheta = sqrt(1.0 - CosTheta * CosTheta);

  vec3 H;
  H.x = SinTheta * cos(Phi);
  H.y = CosTheta;
  H.z = SinTheta * sin(Phi);

  vec3 UpVector = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
  vec3 TangentX = normalize(cross(UpVector, N));
  vec3 TangentZ = cross(N, TangentX);

  return TangentX * H.x + N * H.y + TangentZ * H.z;
}
// ###########################################################################


// Hammersley ################################################################
float radicalInverse_VdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec2 Hammersley(uint i, uint N) {
  return vec2(float(i) / float(N), radicalInverse_VdC(i));
}
// ###########################################################################


void main() {
  vec3 dir = normalize(direction);

  vec3 N = dir;
  vec3 V = dir;

  vec3 PrefilteredColor = vec3(0.0);
  float TotalWeight = 0.0;

  const uint NumSamples = 1024u;
  for (uint i = 0; i < NumSamples; i++) {
    vec2 Xi = Hammersley(i, NumSamples);

    vec3 H = ImportanceSampleGGX(Xi, Roughness, N);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);

    float NoL = max(dot(N, L), 0.0);
    if (NoL > 0.0) {
      PrefilteredColor += SampleColor(L) * NoL;
      TotalWeight += NoL;
    }
  }

  outputColor = vec4(PrefilteredColor / TotalWeight, 1.0);
}
