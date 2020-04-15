#version 460

uniform mat4 Model;
uniform mat4 ModelIT;
uniform mat4 View;
uniform mat4 Projection;
uniform vec2 ProjectionParams; // x: near, y: far

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

out vec3 vWorldNormal;
out vec3 vWorldTangent;
out float vDepth;
out vec2 vUv;

float EncodeDepth(in float viewDepth, in vec2 ProjectionParams)
{
  return (-viewDepth - ProjectionParams.x) / (ProjectionParams.y - ProjectionParams.x);
}

void main()
{
  vWorldNormal = mat3(ModelIT) * normal;
  vWorldTangent = mat3(ModelIT) * tangent;
  vUv = uv;

  vec4 viewPos = View * Model * position;
  vDepth = EncodeDepth(viewPos.z, ProjectionParams);

  gl_Position = Projection * viewPos;
}
