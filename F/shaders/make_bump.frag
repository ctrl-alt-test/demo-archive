uniform int time;

varying vec3 vLight0;
varying vec3 vLight1;
//varying mat3 vLight;

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

// Eclairage de Lambert
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

  float specularIntensity0 = float(diffuseIntensity0 > 0.) * getSpecularIntensity(nLight0, light0Attenuation);
  float specularIntensity1 = float(diffuseIntensity1 > 0.) * getSpecularIntensity(nLight1, light1Attenuation);

  vec4 specularComponent0 = specularIntensity0 * getSpecularColor(0);
  vec4 specularComponent1 = specularIntensity1 * getSpecularColor(1);

  vec3 color = albedo.rgb * mix(vec3(1.), ambientComponent.rgb + diffuseComponent0.rgb + diffuseComponent1.rgb, albedo.a) + specularComponent0.rgb + specularComponent1.rgb;
  float glow = max(max(1. - albedo.a, diffuseComponent0.a), specularComponent0.a + specularComponent1.a);

  return vec4(color, 1. - glow);
}

vec4 getBumpSilhouette()
{
  vec4 albedo = getAlbedo_tex();

  vec3 nLight = normalize(vLight1);
  float lightAttenuation = 1.;

  float diffuseIntensity = lightAttenuation * getDiffuseIntensity(nLight);
  vec4 diffuseComponent = getAmbientColor(0) + diffuseIntensity * getDiffuseColor(0);

  vec4 specularComponent = 0.1 * getSpecularColor(0);

  vec3 color = albedo.rgb * mix(vec3(1.), diffuseComponent.rgb, albedo.a) + specularComponent.rgb;
  float glow = 0.6;

  return vec4(color, 1. - glow);
}
