// -*- glsl -*-

uniform sampler2D c2Map;
varying float fogFactor;

vec3 getSpeedColor();

vec2 getTexCoord();
vec3 getNormal();
vec2 vTexCoord;
vec3 nNormal;

vec4 getBump();
vec4 addFog(vec4 color);

void main (void)
{
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec4 color = getBump();

  vec4 c2 = texture2D(c2Map, 2. * gl_TexCoord[1].st);
  color.rgb *= mix(0.96, 1., c2.a);
  color = addFog(color);

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(getSpeedColor(), 1.);
}
