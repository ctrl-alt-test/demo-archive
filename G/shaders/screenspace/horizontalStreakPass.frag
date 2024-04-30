#version 120

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec2 invResolution;
uniform vec2 lensStreakPow;
uniform int pass;

//[
void directionalBlur(sampler2D tex0, sampler2D tex1, vec2 uv, vec2 dir,
		     float power, int pass,
		     out vec4 color0, out vec4 color1);
//]

void main()
{
  vec4 color0;
  vec4 color1;
  vec2 uv = gl_TexCoord[0].xy;

  directionalBlur(tex0, tex1, uv, vec2(invResolution.x, 0.),
		  lensStreakPow.x, pass < 0 ? -pass : pass,
		  color0, color1);
  if (pass < 0)
    color0 = (color0 + color1) / 2.;

  gl_FragData[0] = color0;
  gl_FragData[1] = color1;
}
