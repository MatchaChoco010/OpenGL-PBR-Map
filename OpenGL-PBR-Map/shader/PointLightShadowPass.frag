#version 460

in vec4 worldFragPos;

uniform vec3 worldLightPos;
uniform float range;

void main()
{
  float lightDistance = length(worldFragPos.xyz - worldLightPos);
  lightDistance = lightDistance / range;
  gl_FragDepth = lightDistance;
}
