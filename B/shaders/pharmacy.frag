// -*- glsl -*-

varying vec2 texCoord;

uniform sampler2D cMap;

uniform int time;

void main(void)
{
  vec4 albedo = texture2D(cMap, texCoord);

  float t = float(time) / 1500.;
  float filter = abs(sin(t));
  float col = smoothstep(0.2, 0., abs(albedo.g - filter));
  albedo = vec4(0., col, 0., col);

  gl_FragColor = albedo;
}
