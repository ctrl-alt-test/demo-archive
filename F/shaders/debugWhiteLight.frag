//
// Lumière blanche avec très faible variation d'intensité
//

uniform vec2 invResolution;

varying vec3 vViewVec;
varying vec3 vLight0;
varying vec3 vLight1;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nViewVec;

vec4 getAlbedo_tex();

vec2 getTexCoord();
vec3 getNormal();
float getVSMAttenuation();
float getDiffuseIntensity(vec3 nLight);
float getSpecularIntensity(vec3 nLight, float lightAttenuation);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();

  nNormal = (gl_FragCoord.x * invResolution.x < 0.5 ?
	     vec3(0., 0., 1.) :
	     getNormal());

  vec4 albedo = getAlbedo_tex();

  float shadowAttenuation = getVSMAttenuation();

  vec3 nLight0 = normalize(vLight0);
  vec3 nLight1 = normalize(vLight1);
  float diffuse0 = pow(getDiffuseIntensity(nLight0), 0.3);
  float diffuse1 = pow(getDiffuseIntensity(nLight1), 0.3);
  float specular0 = float(diffuse0 > 0.) * getSpecularIntensity(nLight0, 1.);
  float specular1 = float(diffuse1 > 0.) * getSpecularIntensity(nLight1, 1.);

  vec3 darkColor = vec3(0.6);
  vec3 litColor = vec3(1.);
  vec3 color = albedo.rgb * mix(darkColor, litColor, 0.5 * (diffuse0 * shadowAttenuation + diffuse1)) + vec3(specular0 + specular1);

  gl_FragColor = vec4(color, 1.);
}
