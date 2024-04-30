#version 120

uniform sampler2D tex0; // color

varying vec4 vColor;

//[
vec2 getTexCoord_no_bump();
//]

void main(void)
{
  gl_FragColor = vColor;
  gl_FragColor.a *= texture2D(tex0, getTexCoord_no_bump()).a;
}
