#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float roughness;
uniform vec3 ambientLight;
uniform vec3 diffuseColor;
uniform vec3 emissiveColor;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;
uniform vec3 specularColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;
in vec4 viewSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec3 albedo = diffuseColor;
  vec3 f0 = specularColor;

  vec3 color = emissiveColor + mix(albedo * ambientLight, ambientLight, Schlick(f0, E, N));

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    vec3 lightIntensity = attenuation * (lightColors[i] * getDistanceAbsorption(mediumColor, mediumDensity * sqrt(dot(lightVecs[i], lightVecs[i]))));

    color += BlinnPhong(E, normalize(lightVecs[i]), N, lightIntensity, albedo, f0, roughness);
  }
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, 1.);
}
