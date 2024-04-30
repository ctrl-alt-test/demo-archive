#version 330

const int i_NUM_LIGHTS = 5;

uniform float size;
uniform float time;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPosition;
uniform vec4 clipPlane;
uniform vec4 lightPositions[i_NUM_LIGHTS];
uniform vec4 rotation;

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
//]

out vec2 depth;
out vec2 texCoord;
out vec3 lightVecs[i_NUM_LIGHTS];
out vec3 normal;
out vec3 viewVec;
out vec4 shadowCoord[i_NUM_LIGHTS];

mat4 getRotationMatrix(float phi, vec3 v)
{
  mat4 matrix;

  float rcos = cos(phi);
  float rsin = sin(phi);

  matrix[0][0] =        rcos + v.x * v.x * (1 - rcos);
  matrix[0][1] = -v.z * rsin + v.x * v.y * (1 - rcos);
  matrix[0][2] =  v.y * rsin + v.x * v.z * (1 - rcos);
  matrix[0][3] = 0.0;

  matrix[1][0] =  v.z * rsin + v.y * v.x * (1 - rcos);
  matrix[1][1] =        rcos + v.y * v.y * (1 - rcos);
  matrix[1][2] = -v.x * rsin + v.y * v.z * (1 - rcos);
  matrix[1][3] = 0.0;

  matrix[2][0] = -v.y * rsin + v.z * v.x * (1 - rcos);
  matrix[2][1] =  v.x * rsin + v.z * v.y * (1 - rcos);
  matrix[2][2] =        rcos + v.z * v.z * (1 - rcos);
  matrix[2][3] = 0.0;

  matrix[3][0] = 0.0;
  matrix[3][1] = 0.0;
  matrix[3][2] = 0.0;
  matrix[3][3] = 1.0;

  return matrix;
}

void main()
{
  mat4 rotationMatrix = getRotationMatrix(rotation.w * time, rotation.xyz);
  vec4 movedVertex = rotationMatrix * (vec4(size, size, size, 1.0) * vec4(vertexPosition, 1.0));

  texCoord = vertexTexCoord;
  normal = normalize(mat3(modelMatrix) * (mat3(rotationMatrix) * vertexNormal));
  vec4 worldSpacePosition = modelMatrix * vec4(vertexPosition, 1.0);
  vec4 viewSpacePosition = modelViewMatrix * movedVertex;
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
 
  gl_Position = projectionMatrix * viewSpacePosition;
  gl_ClipDistance[0] = dot(viewSpacePosition.xyz, clipPlane.xyz) - clipPlane.w;
  depth = gl_Position.zw;
}
