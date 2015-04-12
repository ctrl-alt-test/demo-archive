// -*- glsl -*-

uniform sampler2D cMap;

varying vec4 vColor;

vec2 getTexCoord();

void main(void)
{
  gl_FragColor = vColor;
  gl_FragColor.a *= texture2D(cMap, getTexCoord()).a;
}
