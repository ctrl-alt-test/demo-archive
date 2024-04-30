#version 330

in vec2 texCoord;
out vec4 fragmentColor;

#include "assets/shaders/debug/debugFunctions.fs"

void main()
{
  fragmentColor = vec4(debugTextureCoordinates(texCoord), 1.);
}
