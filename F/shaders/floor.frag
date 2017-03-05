uniform int time;

varying vec3 vLight0;
varying vec3 vLight1;
//varying mat3 vLight;
varying vec3 vvPos;

uniform sampler2D mMap; // silhouette

//[
vec3 nNormal;

vec4 getAlbedo_tex();

vec4 getAmbientColor(int i);
vec4 getDiffuseColor(int i);
vec4 getSpecularColor(int i);

float getDiffuseIntensity(vec3 nLight);
float getSpecularIntensity(vec3 nLight, float lightAttenuation);

float getDistanceAttenuation(int i, vec3 vLight);
float getVSMAttenuation();
//]

float getHallwayLightIntensity()
{
  float opening = smoothstep(219000., 218000., float(time));
  opening = pow(opening, 2.); // vitesse d'ouverture non linéaire

  float coef = 1.;
  coef *= smoothstep(180., 230., vvPos.x);
  coef *= smoothstep(90., 110., vvPos.z - opening * 140.);
  coef *= smoothstep(210., 190., vvPos.z);

  vec2 p = vec2(vvPos.z / 100. - 0.75, // largeur
                -vvPos.x / 450. + 0.5); // longueur
  coef *= texture2D(mMap, p).a;
  return 1.5 * coef;
}

vec4 getBump()
{
  vec4 albedo = getAlbedo_tex();

  vec3 nLight0 = normalize(vLight0);
  vec3 nLight1 = normalize(vLight1);
  float light0Attenuation = min(getVSMAttenuation(), getDistanceAttenuation(0, vLight0));
  float light1Attenuation = getDistanceAttenuation(1, vLight1);

  float diffuseIntensity0 = light0Attenuation * getDiffuseIntensity(nLight0);
  float diffuseIntensity1 = light1Attenuation * getDiffuseIntensity(nLight1);

  vec4 ambientComponent = getAmbientColor(1);
  vec4 diffuseComponent0 = diffuseIntensity0 * getDiffuseColor(0);
  vec4 diffuseComponent1 = diffuseIntensity1 * getDiffuseColor(1);


  // L'astuce est ici
  vec4 diffuseComponent2 = getHallwayLightIntensity() * getDiffuseColor(0);


  float specularIntensity0 = float(diffuseIntensity0 > 0.) * getSpecularIntensity(nLight0, light0Attenuation);
  float specularIntensity1 = float(diffuseIntensity1 > 0.) * getSpecularIntensity(nLight1, light1Attenuation);

  vec4 specularComponent0 = specularIntensity0 * getSpecularColor(0);
  vec4 specularComponent1 = specularIntensity1 * getSpecularColor(1) * 0.5;

  vec3 color = albedo.rgb * mix(vec3(1.), ambientComponent.rgb + diffuseComponent0.rgb + diffuseComponent1.rgb + diffuseComponent2.rgb, albedo.a) + specularComponent0.rgb + specularComponent1.rgb;
  float glow = max(max(1. - albedo.a, diffuseComponent0.a), specularComponent0.a + specularComponent1.a);

  return vec4(color, 1. - glow);
}
