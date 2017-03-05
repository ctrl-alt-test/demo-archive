
uniform sampler2D cMap;

varying vec4 vColor;
//[
vec2 getTexCoord_no_bump();
//]

void main(void)
{
  gl_FragColor = vColor;
  gl_FragColor.a *= texture2D(cMap, getTexCoord_no_bump()).a;
}
