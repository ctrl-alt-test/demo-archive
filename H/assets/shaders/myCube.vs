#version 330

const int i_NUM_LIGHTS = 5;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 clipPlane;
uniform vec4 lightPositions[i_NUM_LIGHTS];

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
//]
out vec3 vertexCoord;
out vec2 texCoord;
out vec3 lightVecs[i_NUM_LIGHTS];
out vec3 normal;
out vec3 viewVec;

void main()
{
  texCoord = vertexTexCoord;
  vertexCoord = vertexPosition;
  normal = normalize(mat3(modelViewMatrix) * vertexNormal);
  vec4 viewSpacePosition = modelViewMatrix * vec4(vertexPosition, 1.0);
  viewVec = -viewSpacePosition.xyz;

  lightVecs[0] = lightPositions[0].xyz - lightPositions[0].w * viewSpacePosition.xyz;
  lightVecs[1] = lightPositions[1].xyz - lightPositions[1].w * viewSpacePosition.xyz;
  lightVecs[2] = lightPositions[2].xyz - lightPositions[2].w * viewSpacePosition.xyz;
  lightVecs[3] = lightPositions[3].xyz - lightPositions[3].w * viewSpacePosition.xyz;
  lightVecs[4] = lightPositions[4].xyz - lightPositions[4].w * viewSpacePosition.xyz;

  gl_Position = projectionMatrix * viewSpacePosition;
  gl_ClipDistance[0] = dot(viewSpacePosition.xyz, clipPlane.xyz) - clipPlane.w;
}
