#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;

void main()
{
  vec4 worldVertex = modelViewMatrix * vec4(vertexPosition, 1.);
  gl_Position = projectionMatrix * worldVertex;
}
