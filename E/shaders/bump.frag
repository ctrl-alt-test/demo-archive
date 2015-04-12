// -*- glsl -*-

vec2 getTexCoord();
vec3 getNormal();
vec2 vTexCoord;
vec3 nNormal;

vec4 getBump();
vec4 addFog(vec4 color);

vec3 getSpeedColor();

void main (void)
{
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec4 color = getBump();
  color = addFog(color);

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(getSpeedColor(), 1.);
}
