//
// Affiche une représentation des normales
//

uniform vec2 invResolution;

varying vec3 vViewVec;
varying vec3 vNormal;
varying mat3 tanSpace;

//[
vec2 vTexCoord;
vec3 nViewVec;

vec2 getTexCoord();
vec3 getNormal();
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();

  vec3 flatNormal = normalize(vNormal);
  vec3 bumpNormal =  tanSpace * getNormal();

  vec3 nNormal = gl_FragCoord.x * invResolution.x < 0.5 ? flatNormal : bumpNormal;

  gl_FragColor = vec4(0.5 + 0.5 * nNormal, 1.);
}
