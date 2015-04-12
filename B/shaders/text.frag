// -*- glsl -*-

varying vec4 vColor;
varying vec2 texCoord;

uniform sampler2D cMap;
//uniform sampler2D sMap;
//uniform float trans;

void main(void)
{
  gl_FragColor = vec4(vColor.rgb, vColor.a * texture2D(cMap, texCoord).a);
  /*
  vec4 col = vec4(gl_Color.rgb, gl_Color.a * texture2D(cMap, texCoord).a);
  col.a *= smoothstep(0., 0.3, trans);
  //  float trans2 = smoothstep(0.4, 1., trans);
  //  col.a *= smoothstep(trans2-0.03, trans2, texture2D(sMap, texCoord).x);
  col.a *= smoothstep(1., 0.7, trans);
  gl_FragColor = col;
  */
//  gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * texture2D(cMap, texCoord).a);
}
