#version 330

const int i_NUM_LIGHTS = 5;

uniform float particleSize;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform sampler2D texture0;
uniform ivec2 computeBufferSize;
uniform vec3 cameraPosition;
uniform vec4 clipPlane;
uniform vec4 lightPositions[i_NUM_LIGHTS];

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
//]

out vec2 depthRange;
out vec2 texCoord;
out vec3 bitangent;
out vec3 lightVecs[i_NUM_LIGHTS];
out vec3 normal;
out vec3 tangent;
out vec3 viewVec;
out vec4 shadowCoord[i_NUM_LIGHTS];
out vec4 viewSpacePosition;
out vec4 worldSpacePosition;

void main()
{
  int line = gl_InstanceID / computeBufferSize.x;
  int col = gl_InstanceID % computeBufferSize.x;
  vec4 particleData = texelFetch(texture0, ivec2(col, line), 0);

  // FIXME: the quad has reverse y texture coordinates.
  texCoord = vec2(vertexTexCoord.x, 1. - vertexTexCoord.y);

  tangent = mat3(modelMatrix) * (transpose(mat3(viewMatrix)) * vec3(1., 0., 0.));
  bitangent = mat3(modelMatrix) * (transpose(mat3(viewMatrix)) * vec3(0., 1., 0.));
  normal = mat3(modelMatrix) * (transpose(mat3(viewMatrix)) * vec3(0., 0., 1.));

  vec2 corner = vertexPosition.xy - 0.5;
  vec3 center = particleData.xyz;
  float size = particleData.w * particleSize * mix(0.5, 1.5, fract(10000. * float(gl_InstanceID)));
  vec3 vertexOffset = transpose(mat3(viewMatrix)) * vec3(size * corner, 0.);
  worldSpacePosition = modelMatrix * (vec4(center + vertexOffset, 1.));
  viewSpacePosition = modelViewMatrix * vec4(center, 1.) + vec4(size * corner, 0., 0.);
  viewVec = cameraPosition - worldSpacePosition.xyz;

  vec4 clipSpacePositionClose = projectionMatrix * viewSpacePosition;
  vec4 clipSpacePositionFar = projectionMatrix * (viewSpacePosition + vec4(0., 0., size, 0.));
  depthRange.x = clipSpacePositionFar.z - clipSpacePositionClose.z;
  depthRange.y = clipSpacePositionClose.w;

  lightVecs[0] = lightPositions[0].xyz - lightPositions[0].w * worldSpacePosition.xyz;
  lightVecs[1] = lightPositions[1].xyz - lightPositions[1].w * worldSpacePosition.xyz;
  lightVecs[2] = lightPositions[2].xyz - lightPositions[2].w * worldSpacePosition.xyz;
  lightVecs[3] = lightPositions[3].xyz - lightPositions[3].w * worldSpacePosition.xyz;
  lightVecs[4] = lightPositions[4].xyz - lightPositions[4].w * worldSpacePosition.xyz;

  shadowCoord[0] = modelLightProjectionMatrix[0] * vec4(center, 1.0);
  shadowCoord[1] = modelLightProjectionMatrix[1] * vec4(center, 1.0);
  shadowCoord[2] = modelLightProjectionMatrix[2] * vec4(center, 1.0);
  shadowCoord[3] = modelLightProjectionMatrix[3] * vec4(center, 1.0);
  shadowCoord[4] = modelLightProjectionMatrix[4] * vec4(center, 1.0);

  gl_Position = projectionMatrix * viewSpacePosition;
  gl_ClipDistance[0] = dot(viewSpacePosition.xyz, clipPlane.xyz) - clipPlane.w;
}
