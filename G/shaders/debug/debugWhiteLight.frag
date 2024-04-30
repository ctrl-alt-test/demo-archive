#version 120

//
// Lumière blanche avec très faible variation d'intensité
//

uniform vec2 invResolution;

varying vec3 vViewVec;
varying vec3 vLightVec[2];

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

vec3 getAlbedo_tex();

vec2 getTexCoord();
vec3 getNormal();
float getVSMAttenuation();
float getDiffuseIntensity(vec3 nLightVec);
float getSpecularIntensity(vec3 nLightVec);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();

  nNormal = (gl_FragCoord.x * invResolution.x < 0.5 ?
	     vec3(0., 0., 1.) :
	     getNormal());

  vec3 albedo = getAlbedo_tex();

  float shadowAttenuation = getVSMAttenuation();

  vec3 nLightVec0 = normalize(vLightVec[0]);
  vec3 nLightVec1 = normalize(vLightVec[1]);
  float diffuse0 = getDiffuseIntensity(nLightVec0);
  float diffuse1 = getDiffuseIntensity(nLightVec1);
  float specular0 = float(diffuse0 > 0.) * getSpecularIntensity(nLightVec0);
  float specular1 = float(diffuse1 > 0.) * getSpecularIntensity(nLightVec1);

  vec3 darkColor = vec3(0.6);
  vec3 litColor = vec3(1.);
  vec3 color = albedo * mix(darkColor, litColor, 0.5 * (diffuse0 * shadowAttenuation + diffuse1)) + vec3(specular0 + specular1);

  gl_FragColor = vec4(color, 1.);
}
