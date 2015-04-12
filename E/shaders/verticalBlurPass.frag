//
#version 120

uniform sampler2D cMap;

vec4 verticalGaussianBlur(sampler2D tex, vec2 uv, float intensity);

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  gl_FragColor = verticalGaussianBlur(cMap, uv, 1./256.);
}
