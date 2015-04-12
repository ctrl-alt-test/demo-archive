// -*- glsl -*-

varying vec2 texCoord;

uniform sampler2D cMap;

uniform int time;

void main(void)
{
  float t = sin(texCoord.x + texCoord.y + float(time) / 500.) * 0.3 + 0.3;

  vec4 albedo = texture2D(cMap, texCoord);
  albedo.a = t;

  gl_FragColor = albedo;
}
