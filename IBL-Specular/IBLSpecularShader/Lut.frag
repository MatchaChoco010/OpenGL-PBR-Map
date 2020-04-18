#version 460

in vec2 vUv;

layout (location = 0) out vec2 outputColor;

const float PI = 3.14159265358979323846;


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

float G1(float Roughness, float NoV) {
  float k = Roughness * Roughness / 2.0;
  return NoV / (NoV * (1 - k) + k);
}

float G_Smith(float Roughness, float NoV, float NoL) {
  return G1(Roughness, NoV) * G1(Roughness, NoL);
}

void main()
{
  float NoV = vUv.x;
  float Roughness = vUv.y;

  vec3 V;
  V.x = sqrt(1.0 - NoV * NoV);
  V.y = NoV;
  V.z = 0.0;

  vec3 N = vec3(0.0, 1.0, 0.0);

  float A = 0.0;
  float B = 0.0;

  const uint NumSamples = 1024u;
  for (uint i = 0; i < NumSamples; i++) {
    vec2 Xi = Hammersley(i, NumSamples);
    vec3 H = ImportanceSampleGGX(Xi, Roughness, N);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);

    float NoL = max(L.y, 0);
    float NoH = max(H.y, 0);
    float VoH = max(dot(V, H), 0);

    if (NoL > 0) {
      float G = G_Smith(Roughness, NoV, NoL);

      float G_Vis = G * VoH / (NoH * NoV);
      float Fc = pow(1.0 - VoH, 5.0);
      A += (1.0 - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }

  A /= float(NumSamples);
  B /= float(NumSamples);
  outputColor = vec2(A, B);
}
