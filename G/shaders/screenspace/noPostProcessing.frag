#version 120

uniform sampler2D tex0; // color

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  gl_FragColor = texture2D(tex0, uv);
}
