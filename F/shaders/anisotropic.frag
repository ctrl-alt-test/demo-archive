uniform float shininess;
uniform float trans;

varying vec3 vLight0;
varying vec3 vLight1;
varying vec3 vViewVec;
varying vec3 vSpeed;
varying float fogFactor;

//[
vec2 vTexCoord;
vec3 nNormal;
vec3 nTangent;
vec3 nViewVec;

vec2 getTexCoord();
vec3 getNormal();
vec4 getAlbedo_tex();

vec4 getAmbientColor(int i);
vec4 getDiffuseColor(int i);
vec4 getSpecularColor(int i);

vec3 getSpeedColor(vec3 speed);

float getDiffuseIntensity(vec3 nLight);
float getAnisotropicSpecularIntensity(vec3 nLight, float lightAttenuation);

float getDistanceAttenuation(int i, vec3 vLight);
float getVSMAttenuation();

vec4 addFog(vec4 color, float intensity);
//]

void main()
{
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();
  nNormal = getNormal();
  nTangent = vec3(1., 0., 0.);

  vec4 albedo = getAlbedo_tex();

  vec3 nLight0 = normalize(vLight0);
  vec3 nLight1 = normalize(vLight1);
  float lightAttenuation = min(getVSMAttenuation(), getDistanceAttenuation(0, vLight0));

  float diffuseIntensity0 = 0.5 * lightAttenuation * getDiffuseIntensity(nLight0);
  float diffuseIntensity1 = 0.5 * getDiffuseIntensity(nLight1);

  vec4 ambientComponent = vec4(vec3(0.05), 1.);// getAmbientColor(1);
  vec4 diffuseComponent0 = diffuseIntensity0 * getDiffuseColor(0);
  vec4 diffuseComponent1 = diffuseIntensity1 * getDiffuseColor(1);

  float specularIntensity0 = float(diffuseIntensity0 > 0.) * getAnisotropicSpecularIntensity(nLight0, lightAttenuation);
  float specularIntensity1 = float(diffuseIntensity1 > 0.) * getAnisotropicSpecularIntensity(nLight1, 1.);

  vec4 specularComponent0 = specularIntensity0 * getSpecularColor(0);
  vec4 specularComponent1 = specularIntensity1 * getSpecularColor(1);

  vec3 color = albedo.rgb * mix(vec3(1.), ambientComponent.rgb + diffuseComponent0.rgb + diffuseComponent1.rgb, albedo.a) + specularComponent0.rgb + specularComponent1.rgb;
  float glow = max(max(1. - albedo.a, diffuseComponent0.a), specularComponent0.a + specularComponent1.a);

  glow += trans * 0.75;
  gl_FragData[0] = addFog(vec4(color, 1. - glow), fogFactor);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
