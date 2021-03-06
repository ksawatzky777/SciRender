#version 440

in VERT_OUT
{
  vec3 fNearPoint;
  vec3 fFarPoint;
  mat4 fInvViewProj;
} fragIn;

uniform mat4 viewProj;
layout(binding = 0) uniform sampler2DShadow gDepth;

layout(location = 1) out vec4 fragColour;

// Helper functions.
vec3 unProject(vec3 position, mat4 invVP);
float xzTransparency(vec3 xzFragPos3D, float scale);

// Compute the x-z grid.
vec4 grid(vec3 xzFragPos3D, float scale);

// Compute the y-axis line.
float yAxis(vec3 xyFragPos3D, float scale);

void main()
{
  float t = -fragIn.fNearPoint.y / (fragIn.fFarPoint.y - fragIn.fNearPoint.y);
  float s = -fragIn.fNearPoint.z / (fragIn.fFarPoint.z - fragIn.fNearPoint.z);
  vec3 xzFragPos3D = fragIn.fNearPoint + t * (fragIn.fFarPoint - fragIn.fNearPoint);
  vec3 xyFragPos3D = fragIn.fNearPoint + s * (fragIn.fFarPoint - fragIn.fNearPoint);

  // Compute the depth of the current fragment along both the x-y and x-z planes
  // [0, 1].
  vec4 xzFragClipPos = viewProj * vec4(xzFragPos3D, 1.0);
  float xzFragDepth = 0.5 * (xzFragClipPos.z / xzFragClipPos.w) + 0.5;
  vec4 xyFragClipPos = viewProj * vec4(xyFragPos3D, 1.0);
  float xyFragDepth = 0.5 * (xyFragClipPos.z / xyFragClipPos.w) + 0.5;
  // Fetch the scene depth test for both planes.
  vec2 fTexCoords = gl_FragCoord.xy / textureSize(gDepth, 0);
  float xzSceneDepth = texture(gDepth, vec3(fTexCoords, xzFragDepth)).r;
  float xySceneDepth = texture(gDepth, vec3(fTexCoords, xyFragDepth)).r;

  vec3 screenNear = unProject(vec3(0.0), fragIn.fInvViewProj);
  float xzFalloff = max(1.5 - 0.1 * length(xzFragPos3D - screenNear), 0.0);
  float xyFalloff = max(1.5 - 0.1 * length(xyFragPos3D - screenNear), 0.0);

  fragColour = (grid(xzFragPos3D, 10.0) + grid(xzFragPos3D, 1.0)) * float(t > 0.0) * xzFalloff * xzSceneDepth;
  fragColour.y = yAxis(xyFragPos3D, 10.0) * float(s > 0.0) * xySceneDepth * xyFalloff
    + (xzTransparency(xzFragPos3D, 10.0) + xzTransparency(xzFragPos3D, 1.0)) * float(t > 0.0) * xzFalloff * xzSceneDepth;
}

vec3 unProject(vec3 position, mat4 invVP)
{
  vec4 temp = invVP * vec4(position, 1.0);
  return temp.xyz / temp.w;
}

float xzTransparency(vec3 xzFragPos3D, float scale)
{
  vec2 coord = xzFragPos3D.xz * scale;
  vec2 derivative = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
  float line = min(grid.x, grid.y);

  float minimumz = min(derivative.y, 1.0);
  float minimumx = min(derivative.x, 1.0);

  float transparency = 1.0 - min(line, 1.0);
  return transparency * 0.4;
}

vec4 grid(vec3 xzFragPos3D, float scale)
{
  vec2 coord = xzFragPos3D.xz * scale;
  vec2 derivative = fwidth(coord);

  float minimumz = min(derivative.y, 1.0);
  float minimumx = min(derivative.x, 1.0);

  float transparency = xzTransparency(xzFragPos3D, scale);
  vec4 color = vec4(transparency, transparency, transparency, 1.0);

  // Z axis.
  if (xzFragPos3D.x > -0.1 * minimumx && xzFragPos3D.x < 0.1 * minimumx)
    color.z = 1.0;
  // X axis.
  if (xzFragPos3D.z > -0.1 * minimumz && xzFragPos3D.z < 0.1 * minimumz)
    color.x = 1.0;

  return color;
}

float yAxis(vec3 xyFragPos3D, float scale)
{
  vec2 coord = xyFragPos3D.xy * scale;
  vec2 derivative = fwidth(coord);

  float minimumx = min(derivative.x, 1);

  return float(xyFragPos3D.x > -0.1 * minimumx && xyFragPos3D.x < 0.1 * minimumx);
}
