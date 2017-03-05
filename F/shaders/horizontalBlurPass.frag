//
#version 120

uniform sampler2D cMap;
uniform vec2 invResolution;

//[
vec4 horizontalGaussianBlur(sampler2D tex, vec2 uv, float intensity);
//]

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  gl_FragColor = horizontalGaussianBlur(cMap, uv, invResolution.x);
}
