#version 330

uniform float amplitude;
uniform float init;
uniform float time;
uniform sampler2D randomTexture;
uniform sampler2D texture0;
uniform vec2 invResolution;
uniform vec2 resolution;
uniform vec2 speed;
uniform vec3 volumeSize;

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/randFunctions.fs"

vec3 randPos(vec2 uv)
{
  // The trick here, is that particles are ordered along x.
  return vec3(1. - uv.y, texelFetch(randomTexture, ivec2(resolution * uv) % 256, 0).rg + uv/64.);
}

void main()
{
  vec3 oldPos = texture2D(texture0, texCoord).xyz;
  vec3 newPos = oldPos;

  if (init != 0.)
  {
    newPos = volumeSize * (randPos(texCoord) * 2. - 1.);
  }
  else
  {
    newPos.y += speed.y;
    newPos.x += speed.x * sin(amplitude * time + 6.283 * texCoord.x);
    newPos.z += speed.x * cos(amplitude * time + 6.283 * texCoord.x);

    // The rendering will assume particles to be ordered along x.
    // Be careful not to change x beyond a local offset.

    if (abs(newPos.y) > volumeSize.y)
    {
      newPos.y = -sign(newPos.y) * volumeSize.y;
    }
    if (abs(newPos.z) > volumeSize.z)
    {
      newPos.z = -sign(newPos.z) * volumeSize.z;
    }
  }
  fragmentColor = vec4(newPos, 1.);
}
