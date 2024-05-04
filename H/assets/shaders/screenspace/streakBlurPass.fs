#version 330

uniform int pass;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec2 direction;
uniform vec3 streakPower;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor0;
layout(location = 1) out vec4 fragmentColor1;
//]

#include "assets/shaders/screenspace/blurFunctions.fs"

void main()
{
  vec3 color0;
  vec3 color1;

  KawaseStreakBlur(texture0, texture1, texCoord, direction,
		   streakPower,
		   abs(pass),
		   color0, color1);

  // Negative pass indicates it's the last pass
  if (pass < 0)
    color0 = (color0 + color1) / 2.;

  fragmentColor0 = vec4(color0, 1.);
  fragmentColor1 = vec4(color1, 1.);
}
