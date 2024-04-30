#version 330

uniform float sharpness;
uniform float zFar;
uniform float zNear;
uniform sampler2D depthMap;
uniform sampler2D texture0;
uniform vec2 direction;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/screenspace/blurFunctions.fs"

void main()
{
  fragmentColor = directionalBilateralGaussianBlur(texture0, depthMap, texCoord, direction, zNear, zFar, sharpness);
}
