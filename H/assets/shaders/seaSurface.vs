#version 330

const int i_NUM_LIGHTS = 5;

uniform float submersibleDepth;
uniform float time;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform sampler2D randomTexture;
uniform vec3 cameraPosition;
uniform vec4 lightPositions[i_NUM_LIGHTS];
uniform vec4 waveParameters;

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
//]

out vec2 texCoord;
out vec3 lightVecs[i_NUM_LIGHTS];
out vec3 normal;
out vec3 tangent;
out vec3 viewSpaceNormal;
out vec3 viewSpaceTangent;
out vec3 viewVec;
out vec4 clipSpacePosition;
out vec4 shadowCoord[i_NUM_LIGHTS];
out vec4 objectSpacePosition;
out vec4 viewSpacePosition;
out vec4 worldSpacePosition;

#include "assets/shaders/randFunctions.fs"
#include "assets/shaders/waterFunctions.fs"

vec2 segment(vec2 p, vec2 a, vec2 b)
{
  vec2 ap = p - a;
  vec2 ab = b - a;
  float h = clamp(dot(ap, ab) / dot(ab, ab), 0., 1.);
  return vec2(length(ap - ab * h), h);
}

vec3 getDisplacement(vec2 p)
{
  vec3 wave = vec3(0);
  for (int i = 0; i < 8; ++i)
  {
    vec2 hash = hash2DTex(randomTexture, i/255.);
    float direction = hash.x * 2. * PI;
    float A = 0.5 * mix(waveParameters.x, waveParameters.y, hash.y);
    wave += GerstnerWave(time, p, A, A * waveParameters.z, waveParameters.a, vec2(sin(direction), cos(direction)));
  }

  return wave;
}

float getSplash(vec2 p)
{
  float splashStart = 22.;
  float splashPeak = 22.7;
  float splashEnd = 24.5;
  float t1 = clamp((time - splashStart)/(splashEnd - splashStart), 0., 1);
  float t2 = clamp((time - splashPeak)/(splashEnd - splashPeak), 0., 1);
  float ripple = 0.;
  float splashShape = 0.;

  if (time >= splashStart && time <= splashEnd)
  {
    // Splash.
    float splashMaxRadius = mix(1.5, 3., clamp((time-splashPeak)/(splashEnd-splashPeak), 0., 1.));
    float splashDepth = min(0., submersibleDepth - 1.3);
    splashDepth = mix(splashDepth, 0., mix(1.8*t2, 1., t2));
    vec2 a = vec2(0, 2.25);
    vec2 d = segment(p, a, -a);
    splashShape = mix(splashDepth, 0., smoothstep(1.5 - 3.*t1, 2. - 3.*t2, d.x));

    // Ripple.
    d.x = length(p) - 2. - 12. * t1;
    ripple = 1.5 * (1. - t1) * smoothstep(-2., -1., d.x) * smoothstep(0., -1., d.x);
  }

  return ripple + splashShape;
}

void main()
{
  texCoord = vertexTexCoord;

  vec3 displacedVertex = vertexPosition;
  displacedVertex.y += getSplash(vertexPosition.xz);

  objectSpacePosition = vec4(vertexPosition, 1.0);
  worldSpacePosition = modelMatrix * vec4(displacedVertex, 1.0);
  worldSpacePosition.xyz += getDisplacement(worldSpacePosition.xz);
  viewSpacePosition = viewMatrix * worldSpacePosition;

  // Compute normal and tangent.
  vec2 pb = worldSpacePosition.xz + vec2(.05, .0);
  vec2 pc = worldSpacePosition.xz + vec2(.0, -.05);

  vec3 A = vec3(worldSpacePosition.x, 0., worldSpacePosition.z) +   getDisplacement(worldSpacePosition.xz);
  vec3 B = vec3(pb.x, 0., pb.y) + getDisplacement(pb);
  vec3 C = vec3(pc.x, 0., pc.y) + getDisplacement(pc);

  vec3 modelSpaceTangent = normalize(B - A);
  vec3 modelSpaceNormal = normalize(cross(modelSpaceTangent, normalize(C - A)));

  normal = mat3(modelMatrix) * modelSpaceNormal;
  tangent = mat3(modelMatrix) * modelSpaceTangent;
  viewSpaceNormal = normalize(mat3(modelViewMatrix) * modelSpaceNormal);
  viewSpaceTangent = normalize(mat3(modelViewMatrix) * modelSpaceTangent);

  viewVec = cameraPosition - worldSpacePosition.xyz;

  lightVecs[0] = lightPositions[0].xyz - lightPositions[0].w * worldSpacePosition.xyz;
  lightVecs[1] = lightPositions[1].xyz - lightPositions[1].w * worldSpacePosition.xyz;
  lightVecs[2] = lightPositions[2].xyz - lightPositions[2].w * worldSpacePosition.xyz;
  lightVecs[3] = lightPositions[3].xyz - lightPositions[3].w * worldSpacePosition.xyz;
  lightVecs[4] = lightPositions[4].xyz - lightPositions[4].w * worldSpacePosition.xyz;

  shadowCoord[0] = modelLightProjectionMatrix[0] * vec4(vertexPosition, 1.0);
  shadowCoord[1] = modelLightProjectionMatrix[1] * vec4(vertexPosition, 1.0);
  shadowCoord[2] = modelLightProjectionMatrix[2] * vec4(vertexPosition, 1.0);
  shadowCoord[3] = modelLightProjectionMatrix[3] * vec4(vertexPosition, 1.0);
  shadowCoord[4] = modelLightProjectionMatrix[4] * vec4(vertexPosition, 1.0);

  clipSpacePosition = projectionMatrix * viewSpacePosition;
  gl_Position = projectionMatrix * viewSpacePosition;
}
