//
// Shader pour les cordes
//

uniform float shininess;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vLight0;
varying vec3 vLight1;
varying vec3 vViewVec;
varying vec3 vSpeed;
varying float fogFactor;
varying vec4 vColor;

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

float getAnisotropicDiffuseIntensity(vec3 nLight);
float getAnisotropicSpecularIntensity(vec3 nLight, float lightAttenuation);

vec3 getSpeedColor(vec3 speed);

float getDistanceAttenuation(int i, vec3 vLight);
float getShadowAttenuation();

vec4 addFog(vec4 color, float intensity);
//]

void main()
{
  nNormal = normalize(vNormal);
  nTangent = normalize(vTangent);
  nViewVec = normalize(vViewVec);
  vTexCoord = getTexCoord();

  vec4 albedo = vColor * getAlbedo_tex();

  vec3 nLight0 = normalize(vLight0);
  vec3 nLight1 = normalize(vLight1);
  float light0Attenuation = min(getShadowAttenuation(), getDistanceAttenuation(0, vLight0));
  float light1Attenuation = getDistanceAttenuation(1, vLight1);

  float diffuseIntensity0 = light0Attenuation * getAnisotropicDiffuseIntensity(nLight0);
  float diffuseIntensity1 = light1Attenuation * getAnisotropicDiffuseIntensity(nLight1);

  // x0.4 en dur parce que la cyclogratte est dans un coin
  // x0.7 en dur parce que sinon ça fait néon
  vec4 ambientComponent = 0.4 * getAmbientColor(1);
  vec4 diffuseComponent0 = 0.7 * diffuseIntensity0 * getDiffuseColor(0);
  vec4 diffuseComponent1 = 0.4 * diffuseIntensity1 * getDiffuseColor(1);

  float specularIntensity0 = float(diffuseIntensity0 > 0.) * getAnisotropicSpecularIntensity(nLight0, light0Attenuation);
  float specularIntensity1 = float(diffuseIntensity1 > 0.) * getAnisotropicSpecularIntensity(nLight1, light1Attenuation);

  vec4 specularComponent0 = specularIntensity0 * getSpecularColor(0);
  vec4 specularComponent1 = specularIntensity1 * getSpecularColor(1);

  vec3 color = albedo.rgb * mix(vec3(1.), ambientComponent.rgb + diffuseComponent0.rgb + diffuseComponent1.rgb, albedo.a) + specularComponent0.rgb + specularComponent1.rgb;
  float glow = max(max(1. - albedo.a, diffuseComponent0.a), specularComponent0.a + specularComponent1.a);

  gl_FragData[0] = addFog(vec4(color, 1. - glow), fogFactor);
  gl_FragData[1] = vec4(getSpeedColor(vSpeed), 1.);
}
