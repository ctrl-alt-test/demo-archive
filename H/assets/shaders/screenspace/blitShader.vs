#version 330

uniform mat4 projectionMatrix;

//[
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
//]
out vec2 texCoord;

void main()
{
  texCoord = vertexTexCoord;
  gl_Position = projectionMatrix * vec4(vertexPosition, 0.0, 1.0);
}
