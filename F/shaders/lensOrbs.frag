// Version 1.20 pour gl_PointCoord
#version 120

uniform sampler2D cMap;
uniform float fade;
uniform float fadeLuminance;

void main()
{
  vec4 orbs = vec4(gl_Color.rgb, pow(1. - gl_Color.a, 1.5));
  gl_FragColor = vec4(texture2D(cMap, gl_PointCoord).a) *
    mix(vec4(fadeLuminance), orbs, fade);
}
