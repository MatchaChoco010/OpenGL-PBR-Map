#version 460

in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec2 vUv;

layout (location = 0) out vec4 fragment;

uniform vec3 worldCameraPosition;
uniform float emissiveIntensity;
uniform float anisotropic;

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D metallicMap;
layout (binding = 2) uniform sampler2D roughnessMap;
layout (binding = 3) uniform sampler2D normalMap;
layout (binding = 4) uniform sampler2D emissiveMap;


const float PI = 3.1415926536;
const float HALF_MAX = 65504.0;


// Light Info ##############################################################
vec3[3] worldLightPositions = vec3[](
  vec3(0, 5, 2),
  vec3(3, 0, 5),
  vec3(-3, 0, 5)
);

vec3[3] lightColors = vec3[](
  vec3(1, 1, 1),
  vec3(0.5, 0.8, 1),
  vec3(0.5, 1, 0.8)
);

float[3] lightIntensities = float[]( // lm
  1200,
  800,
  600
);

float DistanceAttenuation(float distance, float lightRange)
{
  float EPSILON = 0.01;
  float att = 1.0 / (distance * distance + EPSILON);
  float smoothatt = 1 - pow(distance / lightRange, 4.0);
  smoothatt = max(smoothatt, 0.0);
  smoothatt =  smoothatt * smoothatt;
  return att * smoothatt;
}
// #########################################################################


// GetNormal ###############################################################
vec3 GetNormal() {
  vec3 normal = normalize(vWorldNormal);
  vec3 bitangent = normalize(cross(normal, normalize(vWorldTangent)));
  vec3 tangent = normalize(cross(bitangent, normal));
  mat3 TBN = mat3(tangent, bitangent, normal);
  vec3 normalFromMap = texture(normalMap, vUv).rgb * 2 - 1;
  return normalize(TBN * normalFromMap);
}
// #########################################################################



// BRDF ####################################################################
// vec3 NormalizedLambert(vec3 diffuseColor) {
//   return diffuseColor / PI;
// }

// vec3 F_Schlick(vec3 F0, vec3 H, vec3 V) {
//   return (F0 + (1.0 - F0) * pow(1.0 - max(dot(V, H), 0), 5.0));
// }

// float D_GGX(float a, float NoH) {
//   float a2 = a * a;
//   float NoH2 = NoH * NoH;
//   float d = NoH2 * (a2 - 1.0) + 1.0;
//   return a2 / (PI * d * d);
// }

// float G_SchlickGGX(float a, float NoV) {
//   float EPSILON = 0.001;
//   float k = a * a / 2 + EPSILON;
//   float up = NoV;
//   float down = NoV * (1 - k) + k;
//   return up / down;
// }

// float G_Smith(float a, float NoV, float NoL) {
//   float ggx0 = G_SchlickGGX(1, NoV);
//   float ggx1 = G_SchlickGGX(1, NoL);
//   return ggx0 * ggx1;
// }

// vec3 BRDF(vec3 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 L, vec3 H) {
//   float EPSILON = 0.001;
//   vec3 F0 = mix(vec3(0.04), albedo, metallic);
//   float NoH = max(dot(N, H), 0);
//   float NoV = max(dot(N, V), 0);
//   float NoL = max(dot(N, L), 0);
//   float a = roughness * roughness;

//   // specular
//   vec3 F = F_Schlick(F0, H, V);
//   float D = D_GGX(a, NoH);
//   float G = G_Smith(a, NoV, NoL);
//   vec3 up = F * D * G;
//   float down = max(4.0 * NoV * NoL, EPSILON);
//   vec3 specular = up / down;

//   // diffuse
//   vec3 diffuse = NormalizedLambert(albedo);

//   vec3 kD = vec3(1.0) - F;
//   kD *= 1.0 - metallic;
//   return kD * diffuse + specular;
// }
// #########################################################################


// Disney BRDF #############################################################
// # Copyright Disney Enterprises, Inc.  All rights reserved.
// #
// # Licensed under the Apache License, Version 2.0 (the "License");
// # you may not use this file except in compliance with the License
// # and the following modification to it: Section 6 Trademarks.
// # deleted and replaced with:
// #
// # 6. Trademarks. This License does not grant permission to use the
// # trade names, trademarks, service marks, or product names of the
// # Licensor and its affiliates, except as required for reproducing
// # the content of the NOTICE file.
// #
// # You may obtain a copy of the License at
// # http://www.apache.org/licenses/LICENSE-2.0
float SchlickFresnel(float cosTheta)
{
  return pow(clamp((1 - cosTheta), 0, 1), 5.0);
}

float D_GTR1(float NdotH, float a)
{
  float a2 = a * a;
  float tmp = 1 + (a2 - 1) * NdotH * NdotH;
  return (a2 - 1) / (PI * log(a2) * tmp);
}

float D_GTR2aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
  float tmp = (HdotX * HdotX) / (ax * ax) + (HdotY * HdotY) / (ay * ay) + NdotH * NdotH;
  return 1 / (PI * ax * ay * tmp * tmp);
}

float G_GGX(float NdotV, float a)
{
  float a2 = a * a;
  float down = NdotV + sqrt(a2 + NdotV * NdotV - a2 * NdotV * NdotV);
  return 1 / down;
}

float G_GGXaniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
{
  float tmp = VdotX * VdotX * ax * ax + VdotY * VdotY * ay * ay + NdotV * NdotV;
  float down = NdotV + sqrt(tmp);
  return 1 / down;
}

vec3 DisneyBRDF(vec3 L, vec3 V, vec3 N, vec3 H, vec3 X, vec3 Y, vec3 baseColor, float subsurface, float metallic, float specular, float specularTint, float roughness, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatGloss)
{
  float NdotL = dot(N, L);
  float NdotV = dot(N, V);
  if (NdotL < 0 || NdotV < 0) return vec3(0);

  float NdotH = dot(N, H);
  float LdotH = dot(L, H);

  float luminance = 0.3 * baseColor.r + 0.6 * baseColor.g + 0.1 * baseColor.b;
  vec3 C_tint = luminance > 0 ? baseColor / luminance : vec3(1);
  vec3 C_spec = mix(specular * 0.08 * mix(vec3(1), C_tint, specularTint), baseColor, metallic);
  vec3 C_sheen = mix(vec3(1), C_tint, sheenTint);

  // diffuse
  float F_i = SchlickFresnel(NdotL);
  float F_o = SchlickFresnel(NdotV);
  float F_d90 = 0.5 + 2 * LdotH * LdotH * roughness;
  float F_d = mix(1.0, F_d90, F_i) * mix(1.0, F_d90, F_o);

  float F_ss90 = LdotH * LdotH * roughness;
  float F_ss = mix(1.0, F_ss90, F_i) * mix(1.0, F_ss90, F_o);
  float ss = 1.25 * (F_ss * (1 / (NdotL + NdotV) - 0.5) + 0.5);

  float FH = SchlickFresnel(LdotH);
  vec3 F_sheen = FH * sheen * C_sheen;

  vec3 BRDFdiffuse = ((1 / PI) * mix(F_d, ss, subsurface) * baseColor + F_sheen) * (1 - metallic);

  // specular
  float aspect = sqrt(1 - anisotropic * 0.9);
  float roughness2 = roughness * roughness;
  float a_x = max(0.001, roughness2 / aspect);
  float a_y = max(0.001, roughness2 * aspect);
  float D_s = D_GTR2aniso(NdotH, dot(H, X), dot(H, Y), a_x, a_y);
  vec3 F_s = mix(C_spec, vec3(1), FH);
  float G_s = G_GGXaniso(NdotL, dot(L, X), dot(L, Y), a_x, a_y) * G_GGXaniso(NdotV, dot(V, X), dot(V, Y), a_x, a_y);

  vec3 BRDFspecular = G_s * F_s * D_s;

  // clearcoat
  float D_r = D_GTR1(NdotH, mix(0.1, 0.001, clearcoatGloss));
  float F_r = mix(0.04, 1.0, FH);
  float G_r = G_GGX(NdotL, 0.25) * G_GGX(NdotV, 0.25);

  vec3 BRDFclearcoat = vec3(0.25 * clearcoat * G_r * F_r * D_r);

  return BRDFdiffuse + BRDFspecular + BRDFclearcoat;
}
// #########################################################################


// exposure ################################################################
const float exposure = 0.2;
// #########################################################################


// ACES ####################################################################
//
// https://github.com/ampas/aces-dev
//
// Academy Color Encoding System (ACES) software and tools are provided by the
// Academy under the following terms and conditions: A worldwide, royalty-free,
// non-exclusive right to copy, modify, create derivatives, and use, in source
// and binary forms, is hereby granted, subject to acceptance of this license.
//
// Copyright 2018 Academy of Motion Picture Arts and Sciences (A.M.P.A.S.).
// Portions contributed by others as indicated. All rights reserved.
//
// Performance of any of the aforementioned acts indicates acceptance to be
// bound by the following terms and conditions:
//
// * Copies of source code, in whole or in part, must retain the above
//   copyright notice, this list of conditions and the Disclaimer of Warranty.
//
// * Use in binary form must retain the above copyright notice, this list of
//   conditions and the Disclaimer of Warranty in the documentation and/or
//   other materials provided with the distribution.
//
// * Nothing in this license shall be deemed to grant any rights to trademarks,
//   copyrights, patents, trade secrets or any other intellectual property of
//   A.M.P.A.S. or any contributors, except as expressly stated herein.
//
// * Neither the name "A.M.P.A.S." nor the name of any other contributors to
//   this software may be used to endorse or promote products derivative of or
//   based on this software without express prior written permission of
//   A.M.P.A.S. or the contributors, as appropriate.
//
// This license shall be construed pursuant to the laws of the State of
// California, and any disputes related thereto shall be subject to the
// jurisdiction of the courts therein.
//
// Disclaimer of Warranty: THIS SOFTWARE IS PROVIDED BY A.M.P.A.S. AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, AND NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT SHALL
// A.M.P.A.S., OR ANY CONTRIBUTORS OR DISTRIBUTORS, BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, RESITUTIONARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// WITHOUT LIMITING THE GENERALITY OF THE FOREGOING, THE ACADEMY SPECIFICALLY
// DISCLAIMS ANY REPRESENTATIONS OR WARRANTIES WHATSOEVER RELATED TO PATENT OR
// OTHER INTELLECTUAL PROPERTY RIGHTS IN THE ACADEMY COLOR ENCODING SYSTEM, OR
// APPLICATIONS THEREOF, HELD BY PARTIES OTHER THAN A.M.P.A.S.,WHETHER
// DISCLOSED OR UNDISCLOSED.
//
// ############################################################################

// https://github.com/ampas/aces-dev/blob/master/transforms/ctl/README-MATRIX.md

const mat3 sRGB_2_AP0 = mat3(
  0.4397010, 0.0897923, 0.0175440,
  0.3829780, 0.8134230, 0.1115440,
  0.1773350, 0.0967616, 0.8707040
);

const mat3 AP0_2_AP1_MAT = mat3(
  1.4514393161, -0.0765537734, 0.0083161484,
  -0.2365107469, 1.1762296998, -0.0060324498,
  -0.2149285693, -0.0996759264, 0.9977163014
);

const mat3 AP1_2_AP0_MAT = mat3(
  0.6954522414, 0.0447945634, -0.0055258826,
  0.1406786965, 0.8596711185, 0.0040252103,
  0.1638690622, 0.0955343182, 1.0015006723
);

const mat3 AP1_2_XYZ_MAT = mat3(
  0.6624541811, 0.2722287168, -0.0055746495,
  0.1340042065, 0.6740817658, 0.0040607335,
  0.1561876870, 0.0536895174, 1.0103391003
);

const mat3 XYZ_2_AP1_MAT = mat3(
  1.6410233797, -0.6636628587, 0.0117218943,
  -0.3248032942, 1.6153315917, -0.0082844420,
  -0.2364246952, 0.0167563477, 0.9883948585
);

const mat3 XYZ_2_REC709_MAT = mat3(
  3.2409699419, -0.9692436363, 0.0556300797,
  -1.5373831776, 1.8759675015, -0.2039769589,
  -0.4986107603, 0.0415550574, 1.0569715142
);

const mat3 RRT_SAT_MAT = mat3(
  0.9708890, 0.0108892, 0.0108892,
  0.0269633, 0.9869630, 0.0269633,
  0.00214758, 0.00214758, 0.96214800
);

const mat3 ODT_SAT_MAT = mat3(
  0.949056, 0.019056, 0.019056,
  0.0471857, 0.9771860, 0.0471857,
  0.00375827, 0.00375827, 0.93375800
);

const mat3 D60_2_D65_CAT = mat3(
  0.98722400, -0.00759836, 0.00307257,
  -0.00611327, 1.00186000, -0.00509595,
  0.0159533, 0.0053302, 1.0816800
);

float log10(float x)
{
  return log2(x) / log2(10.0);
}

vec3 sRGBToACES(vec3 srgb)
{
  return sRGB_2_AP0 * srgb;
}

float min_f3(vec3 a)
{
  return min(a.x, min(a.y, a.z));
}

float max_f3(vec3 a)
{
  return max(a.x, max(a.y, a.z));
}

float rgb_2_saturation(vec3 rgb)
{
  const float TINY = 1e-10;
  float mi = min_f3(rgb);
  float ma = max_f3(rgb);
  return (max(ma, TINY) - max(mi, TINY)) / max(ma, 1e-2);
}

float rgb_2_yc(vec3 rgb)
{
  const float ycRadiusWeight = 1.75;
  float r = rgb.r;
  float g = rgb.g;
  float b = rgb.b;
  float chroma = sqrt(b * (b - g) + g * (g - r) + r * (r - b));
  return (b + g + r + ycRadiusWeight * chroma) / 3.0;
}

float sigmoid_shaper(float x)
{
  float t = max(1.0 - abs(x / 2.0), 0.0);
  float y = 1.0 + sign(x) * (1.0 - t * t);
  return y / 2.0;
}

float glow_fwd(float ycIn, float glowGainIn, float glowMid)
{
  float glowGainOut;

  if (ycIn <= 2.0 / 3.0 * glowMid)
    glowGainOut = glowGainIn;
  else if (ycIn >= 2.0 * glowMid)
    glowGainOut = 0.0;
  else
    glowGainOut = glowGainIn * (glowMid / ycIn - 1.0 / 2.0);

  return glowGainOut;
}

float rgb_2_hue(vec3 rgb)
{
  float hue;
  if (rgb.r == rgb.g && rgb.g == rgb.b)
    hue = 0.0;
  else
    hue = (180.0 / PI) * atan(sqrt(3.0) * (rgb.g - rgb.b), 2.0 * rgb.r - rgb.g - rgb.b);
  if (hue < 0.0) hue = hue + 360.0;
  return hue;
}

float center_hue(float hue, float centerH)
{
  float hueCentered = hue - centerH;
  if (hueCentered < -180.0) hueCentered = hueCentered + 360.0;
  else if (hueCentered > 180.0) hueCentered = hueCentered - 360.0;
  return hueCentered;
}

float cubic_basis_shaper(float x, float w)
{
  float M[4][4] = {
    { -1.0 / 6, 3.0 / 6, -3.0 / 6, 1.0 / 6 },
    { 3.0 / 6, -6.0 / 6, 3.0 / 6, 0.0 / 6 },
    { -3.0 / 6, 0.0 / 6, 3.0 / 6, 0.0 / 6 },
    { 1.0 / 6, 4.0 / 6, 1.0 / 6, 0.0 / 6}
  };
  float knots[5] = {
    -w / 2.0,
    -w / 4.0,
    0.0,
    w / 4.0,
    w / 2.0
  };

  float y = 0.0;
  if ((x > knots[0]) && (x < knots[4]))
  {
    float knot_coord = (x - knots[0]) * 4.0 / w;
    int j = int(knot_coord);
    float t = knot_coord - j;

    float monomials[4] = { t * t * t, t * t, t, 1.0 };

    if (j == 3)
    {
      y = monomials[0] * M[0][0] + monomials[1] * M[1][0] + monomials[2] * M[2][0] + monomials[3] * M[3][0];
    }
    else if (j == 2)
    {
      y = monomials[0] * M[0][1] + monomials[1] * M[1][1] + monomials[2] * M[2][1] + monomials[3] * M[3][1];
    }
    else if (j == 1)
    {
      y = monomials[0] * M[0][2] + monomials[1] * M[1][2] + monomials[2] * M[2][2] + monomials[3] * M[3][2];
    }
    else if (j == 0)
    {
      y = monomials[0] * M[0][3] + monomials[1] * M[1][3] + monomials[2] * M[2][3] + monomials[3] * M[3][3];
    }
    else
    {
      y = 0.0;
    }
  }

  return y * 3.0 / 2.0;
}

const mat3 M = mat3(
  0.5, -1.0, 0.5,
  -1.0, 1.0, 0.5,
  0.5, 0.0, 0.0
);

float segmented_spline_c5_fwd(float x)
{
  const float coefsLow[6] = { -4.0000000000, -4.0000000000, -3.1573765773, -0.4852499958, 1.8477324706, 1.8477324706 };
  const float coefsHigh[6] = { -0.7185482425, 2.0810307172, 3.6681241237, 4.0000000000, 4.0000000000, 4.0000000000 };
  const vec2 minPoint = vec2(0.18 * exp2(-15.0), 0.0001);
  const vec2 midPoint = vec2(0.18, 0.48);
  const vec2 maxPoint = vec2(0.18 * exp2(18.0), 10000.0);
  const float slopeLow = 0.0;
  const float slopeHigh = 0.0;

  const int N_KNOTS_LOW = 4;
  const int N_KNOTS_HIGH = 4;

  float xCheck = x;
  if (xCheck <= 0.0) xCheck = 0.00006103515; // = pow(2.0, -14.0)

  float logx = log10(xCheck);
  float logy;

  if (logx <= log10(minPoint.x))
  {
    logy = logx * slopeLow + (log10(minPoint.y) - slopeLow * log10(minPoint.x));
  }
  else if ((logx > log10(minPoint.x)) && (logx < log10(midPoint.x)))
  {
    float knot_coord = (N_KNOTS_LOW - 1) * (logx - log10(minPoint.x)) / (log10(midPoint.x) - log10(minPoint.x));
    int j = int(knot_coord);
    float t = knot_coord - j;

    vec3 cf = vec3(coefsLow[j], coefsLow[j + 1], coefsLow[j + 2]);
    vec3 monomials = vec3(t * t, t, 1.0);
    logy = dot(monomials, M * cf);
  }
  else if((logx >= log10(midPoint.x)) && (logx < log10(maxPoint.x)))
  {
    float knot_coord = (N_KNOTS_HIGH - 1) * (logx - log10(midPoint.x)) / (log10(maxPoint.x) - log10(midPoint.x));
    int j = int(knot_coord);
    float t = knot_coord - j;

    vec3 cf = vec3(coefsHigh[j], coefsHigh[j + 1], coefsHigh[j + 2]);
    vec3 monomials = vec3(t * t, t , 1.0);
    logy = dot(monomials, M * cf);
  }
  else
  {
    logy = logx * slopeHigh + (log10(maxPoint.y) - slopeHigh * log10(maxPoint.x));
  }

  return pow(10.0, logy);
}

float segmented_spline_c9_fwd(float x)
{
  const float coefsLow[10] = { -1.6989700043, -1.6989700043, -1.4779000000, -1.2291000000, -0.8648000000, -0.4480000000, 0.0051800000, 0.4511080334, 0.9113744414, 0.9113744414 };
  const float coefsHigh[10] = { 0.5154386965, 0.8470437783, 1.1358000000, 1.3802000000, 1.5197000000, 1.5985000000, 1.6467000000, 1.6746091357, 1.6878733390, 1.6878733390 };
  const vec2 minPoint = vec2(segmented_spline_c5_fwd(0.18 * exp2(-6.5)), 0.02);
  const vec2 midPoint = vec2(segmented_spline_c5_fwd(0.18), 4.8);
  const vec2 maxPoint = vec2(segmented_spline_c5_fwd(0.18 * exp2(6.5)), 48.0);
  const float slopeLow = 0.0;
  const float slopeHigh = 0.04;

  const int N_KNOTS_LOW = 8;
  const int N_KNOTS_HIGH = 8;

  float xCheck = x;
  if (xCheck <= 0.0) xCheck = 1e-4;

  float logx = log10(xCheck);
  float logy;

  if (logx <= log10(minPoint.x))
  {
    logy = logx * slopeLow + (log10(minPoint.y) - slopeLow * log10(minPoint.x));
  }
  else if ((logx > log10(minPoint.x)) && (logx < log10(midPoint.x)))
  {
    float knot_coord = (N_KNOTS_LOW - 1) * (logx - log10(minPoint.x)) / (log10(midPoint.x) - log10(minPoint.x));
    int j = int(knot_coord);
    float t = knot_coord - j;

    vec3 cf = vec3(coefsLow[j], coefsLow[j + 1], coefsLow[j + 2]);
    vec3 monomials = vec3(t * t, t, 1.0);
    logy = dot(monomials, M * cf);
  }
  else if ((logx >= log10(midPoint.x)) && (logx < log10(maxPoint.x)))
  {
    float knot_coord = (N_KNOTS_HIGH - 1) * (logx - log10(midPoint.x)) / (log10(maxPoint.x) - log10(midPoint.x));
    int j = int(knot_coord);
    float t = knot_coord - j;

    vec3 cf = vec3(coefsHigh[j], coefsHigh[j + 1], coefsHigh[j + 2]);
    vec3 monomials = vec3(t * t, t, 1.0);
    logy = dot(monomials, M * cf);
  }
  else
  {
      logy = logx * slopeHigh + (log10(maxPoint.y) - slopeHigh * log10(maxPoint.x));
  }

  return pow(10.0, logy);
}

const float RRT_GLOW_GAIN = 0.05;
const float RRT_GLOW_MID = 0.08;

const float RRT_RED_SCALE = 0.82;
const float RRT_RED_PIVOT = 0.03;
const float RRT_RED_HUE = 0.0;
const float RRT_RED_WIDTH = 135.0;

const float RRT_SAT_FACTOR = 0.96;

vec3 RRT(vec3 aces)
{
  // --- Glow module --- //
  float saturation = rgb_2_saturation(aces);
  float ycIn = rgb_2_yc(aces);
  float s = sigmoid_shaper((saturation - 0.4) / 0.2);
  float addedGlow = 1.0 + glow_fwd(ycIn, RRT_GLOW_GAIN * s, RRT_GLOW_MID);
  aces *= addedGlow;

  // --- Red modifier --- //
  float hue = rgb_2_hue(aces);
  float centeredHue = center_hue(hue, RRT_RED_HUE);
  float hueWeight = cubic_basis_shaper(centeredHue, RRT_RED_WIDTH);

  aces.r += hueWeight * saturation * (RRT_RED_PIVOT - aces.r) * (1.0 - RRT_RED_SCALE);

  // --- ACES to RGB rendering space --- //
  aces = clamp(aces, 0.0, HALF_MAX);  // avoids saturated negative colors from becoming positive in the matrix
  vec3 rgbPre = AP0_2_AP1_MAT * aces;
  rgbPre = clamp(rgbPre, 0.0, HALF_MAX);

  // --- Global desaturation --- //
  rgbPre = RRT_SAT_MAT * rgbPre;

  // --- Apply the tonescale independently in rendering-space RGB --- //
  vec3 rgbPost;
  rgbPost.x = segmented_spline_c5_fwd(rgbPre.x);
  rgbPost.y = segmented_spline_c5_fwd(rgbPre.y);
  rgbPost.z = segmented_spline_c5_fwd(rgbPre.z);

  // --- RGB rendering space to OCES --- //
  vec3 rgbOces = AP1_2_AP0_MAT * rgbPost;

  // Assign OCES RGB to output variables (OCES)
  return rgbOces;
}

vec3 Y_2_linCV(vec3 Y, float Ymax, float Ymin)
{
  return (Y - Ymin) / (Ymax - Ymin);
}

vec3 XYZ_2_xyY(vec3 XYZ)
{
  float divisor = max(dot(XYZ, (1.0).xxx), 1e-4);
  return vec3(XYZ.xy / divisor, XYZ.y);
}

vec3 xyY_2_XYZ(vec3 xyY)
{
  float m = xyY.z / max(xyY.y, 1e-4);
  vec3 XYZ = vec3(xyY.xz, (1.0 - xyY.x - xyY.y));
  XYZ.xz *= m;
  return XYZ;
}

const float DIM_SURROUND_GAMMA = 0.9811;

vec3 darkSurround_to_dimSurround(vec3 linearCV)
{
  vec3 XYZ = AP1_2_XYZ_MAT * linearCV;

  vec3 xyY = XYZ_2_xyY(XYZ);
  xyY.z = clamp(xyY.z, 0.0, HALF_MAX);
  xyY.z = pow(xyY.z, DIM_SURROUND_GAMMA);
  XYZ = xyY_2_XYZ(xyY);

  return XYZ_2_AP1_MAT * XYZ;
}

float moncurve_r(float y, float gamma, float offs)
{
    // Reverse monitor curve
    float x;
    const float yb = pow(offs * gamma / ((gamma - 1.0) * (1.0 + offs)), gamma);
    const float rs = pow((gamma - 1.0) / offs, gamma - 1.0) * pow((1.0 + offs) / gamma, gamma);
    if (y >= yb)
        x = (1.0 + offs) * pow(y, 1.0 / gamma) - offs;
    else
        x = y * rs;
    return x;
}

const float CINEMA_WHITE = 48.0;
const float CINEMA_BLACK = CINEMA_WHITE / 2400.0;

// NOTE: The EOTF is *NOT* gamma 2.4, it follows IEC 61966-2-1:1999
const float DISPGAMMA = 2.4;
const float OFFSET = 0.055;

vec3 ODT_RGBmonitor_100nits_dim(vec3 oces)
{
  // OCES to RGB rendering space
  vec3 rgbPre = AP0_2_AP1_MAT * oces;

  // Apply the tonescale independently in rendering-space RGB
  vec3 rgbPost;
  rgbPost.x = segmented_spline_c9_fwd(rgbPre.x);
  rgbPost.y = segmented_spline_c9_fwd(rgbPre.y);
  rgbPost.z = segmented_spline_c9_fwd(rgbPre.z);

  // Scale luminance to linear code value
  vec3 linearCV = Y_2_linCV(rgbPost, CINEMA_WHITE, CINEMA_BLACK);

    // Apply gamma adjustment to compensate for dim surround
  linearCV = darkSurround_to_dimSurround(linearCV);

  // Apply desaturation to compensate for luminance difference
  linearCV = ODT_SAT_MAT * linearCV;

  // Convert to display primary encoding
  // Rendering space RGB to XYZ
  vec3 XYZ = AP1_2_XYZ_MAT * linearCV;

  // Apply CAT from ACES white point to assumed observer adapted white point
  XYZ = D60_2_D65_CAT * XYZ;

  // CIE XYZ to display primaries
  // linearCV = XYZ_2_DISPLAY_PRI_MAT * XYZ;
  linearCV = XYZ_2_REC709_MAT * XYZ;

  // Handle out-of-gamut values
  // Clip values < 0 or > 1 (i.e. projecting outside the display primaries)
  linearCV = clamp(linearCV, 0.0 , 1.0);

  vec3 outputCV;
  outputCV.x = moncurve_r(linearCV.x, DISPGAMMA, OFFSET);
  outputCV.y = moncurve_r(linearCV.y, DISPGAMMA, OFFSET);
  outputCV.z = moncurve_r(linearCV.z, DISPGAMMA, OFFSET);
  return outputCV;
}

vec3 ACESTonemapping(vec3 aces)
{
  return ODT_RGBmonitor_100nits_dim(RRT(aces));
}
// #########################################################################


// gamma correction ########################################################
const float  SCALE_0= 1.0/12.92;
const float  SCALE_1= 1.0/1.055;
const float  OFFSET_1= 0.055 * SCALE_1;

float LinearToSRGB_F( float color )
{
    color= clamp( color, 0.0, 1.0 );
    if( color < 0.0031308 ){
        return  color * 12.92;
    }
    return  1.055 * pow( color, 0.41666 ) - 0.055;
}

vec3 LinearToSRGB( vec3 color )
{
    return  vec3(
        LinearToSRGB_F( color.x ),
        LinearToSRGB_F( color.y ),
        LinearToSRGB_F( color.z ) );
}
// #########################################################################


void main() {
  vec3 albedo = sRGBToACES(texture(albedoMap, vUv).rgb);
  float metallic = texture(metallicMap, vUv).r;
  float roughness = texture(roughnessMap, vUv).r;
  vec3 emissive = sRGBToACES(texture(emissiveMap, vUv).rgb);

  vec3 color = emissive * emissiveIntensity;

  vec3 N = GetNormal();
  vec3 V = normalize(worldCameraPosition - vWorldPosition);

  vec3 tangent = normalize(vWorldTangent);
  vec3 bitangent = normalize(cross(N, tangent));
  tangent = normalize(cross(bitangent, N));

  float subsurface = 0.0;
  float specular = 0.5;
  float specularTint = 0.0;
  float sheen = 0.0;
  float sheenTint = 0.5;
  float clearcoat = 0.0;
  // float clearcoat = 1.0;
  float clearcoatGloss = 1.0;

  for (int i = 0; i < 3; i++) {
    vec3 L = normalize(worldLightPositions[i] - vWorldPosition);
    vec3 H = normalize(L + V);
    float attenuation =
      DistanceAttenuation(distance(worldLightPositions[i], vWorldPosition), 20.0);
    vec3 irradiance =
      lightIntensities[i] / (4.0 * PI) * attenuation * sRGBToACES(lightColors[i]) * max(dot(N, L), 0);
    // color += BRDF(albedo, metallic, roughness, N, V, L, H) * irradiance;
    color += DisneyBRDF(L, V, N, H, tangent, bitangent, albedo, subsurface, metallic, specular, specularTint, roughness, anisotropic, sheen, sheenTint, clearcoat, clearcoatGloss) * irradiance;
  }

  color *= exposure;

  color = ACESTonemapping(color);

  color = LinearToSRGB(color);

  fragment = vec4(color, 1.0);
}
