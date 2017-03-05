
varying vec3 vViewVec;
varying vec3 vSpeed;
varying float fogFactor;

//[
vec2 getTexCoord();
vec3 getNormal();
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

vec4 getBump();
vec4 addFog(vec4 color, float intensity);

vec3 getSpeedColor(vec3 speed);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();
  nNormal = getNormal();

  vec4 color = getBump();
  color = addFog(color, fogFactor);

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
