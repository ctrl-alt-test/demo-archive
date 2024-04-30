#version 330

uniform sampler2D texture0;
uniform vec2 direction;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/screenspace/blurFunctions.fs"

void main()
{
  fragmentColor = directionalGaussianBlur(texture0, texCoord, direction);
}
