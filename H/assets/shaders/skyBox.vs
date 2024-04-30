#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

//[
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
//]

out vec3 normal;

void main()
{
  normal = vertexNormal;
  vec4 viewSpacePosition = vec4(mat3(modelViewMatrix) * vertexPosition, 1.0);

  gl_Position = projectionMatrix * viewSpacePosition;
}
