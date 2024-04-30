#version 120

uniform sampler2D tex0; // color
uniform vec2 invResolution;
uniform int pass;

//[
vec4 directionalGaussianBlur(sampler2D tex, vec2 uv, vec2 spread);
//]

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  gl_FragColor = directionalGaussianBlur(tex0, uv, vec2(0., invResolution.y * float(1 + pass)));
}
