#version 120

//
// Affiche une représentation de l'éclairage
//

uniform vec2 invResolution;

varying vec3 vViewVec;
varying vec3 vLightVec[2];

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

vec2 getTexCoord();
vec3 getNormal();
float getVSMAttenuation();
float getDiffuseIntensity(vec3 nLight);
float getSpecularIntensity(vec3 nLight);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();

  nNormal = (gl_FragCoord.x * invResolution.x < 0.5 ?
	     vec3(0., 0., 1.) :
	     getNormal());

  float shadowAttenuation = getVSMAttenuation();

  vec3 nLightVec0 = normalize(vLightVec[0]);
  float diffuse = pow(getDiffuseIntensity(nLightVec0), 0.3);
  float specular = float(diffuse > 0.) * clamp(5. * getSpecularIntensity(nLightVec0), 0., 1.);

  vec3 darkColor = vec3(0.1);
  vec3 shadowColor = vec3(.2, 0., 0.);

  vec3 diffColor = vec3(1., 1., 0.);
  vec3 specColor = vec3(0., 0., 1.);
  vec3 litColor = mix(diffColor, specColor, specular);

  vec3 color = mix(shadowColor, mix(darkColor, litColor, diffuse), 0.2 + 0.8 * shadowAttenuation);

  gl_FragColor = vec4(color, 1.);
}
