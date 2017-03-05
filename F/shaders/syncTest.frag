
varying vec3 vSpeed;
varying float fogFactor;

vec2 vTexCoord;
vec3 nNormal;

//[
vec2 getTexCoord();
vec3 getNormal();
vec4 getBump();
vec4 addFog(vec4 color, float intensity);
vec3 getSpeedColor(vec3 speed);
//]

uniform float trans;

void main (void)
{
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec4 color = getBump();
  color = addFog(color, fogFactor);

  gl_FragData[0] = vec4(trans, 0., 0., 1. - trans);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
