#version 330

uniform float time;
uniform mat4 inverseViewProjectionMatrix;
uniform vec2 resolution;
uniform vec3 cameraPosition;
uniform vec3 fogColor;
uniform vec3 skyColor;

in vec2 texCoord;

out vec4 fragmentColor;

#include "assets/shaders/randFunctions.fs"

void main()
{
  // FIXME: there is probably a simpler way to get the view vector.
  vec4 farPosWorldSpace = inverseViewProjectionMatrix * vec4(texCoord * 2. - 1., 1., 1.);
  farPosWorldSpace /= farPosWorldSpace.w;

  vec3 viewVec = normalize(farPosWorldSpace.xyz - cameraPosition);
  float height = clamp(0.04+viewVec.y, 0., 1.);

  vec3 color = mix(fogColor, skyColor, smoothstep(0., 0.8, pow(height, 0.3)));
  fragmentColor = vec4(color, 1.);
}
