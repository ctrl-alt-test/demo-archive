#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float roughness;
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;
uniform vec3 specularColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];
in vec4 viewSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec3 albedo = texture2D(diffuseTexture, texCoord.xy).rgb;
  vec3 f0 = specularColor;

  vec3 color = vec3(0.);
  float shadowAttenuations[i_NUM_LIGHTS];
  shadowAttenuations[0] = getProjectorLightCone(shadowCoord[0]) * getVSMAttenuation(shadowMaps[0], shadowCoord[0]);
  shadowAttenuations[1] = getProjectorLightCone(shadowCoord[1]) * getVSMAttenuation(shadowMaps[1], shadowCoord[1]);
  shadowAttenuations[2] = getProjectorLightCone(shadowCoord[2]) * getVSMAttenuation(shadowMaps[2], shadowCoord[2]);
  shadowAttenuations[3] = getProjectorLightCone(shadowCoord[3]) * getVSMAttenuation(shadowMaps[3], shadowCoord[3]);
  shadowAttenuations[4] = getProjectorLightCone(shadowCoord[4]) * getVSMAttenuation(shadowMaps[4], shadowCoord[4]);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = shadowAttenuations[i] / (1. + dot(lightVecs[i], lightVecs[i]));
    vec3 lightIntensity = attenuation * (lightColors[i] * getDistanceAbsorption(mediumColor, mediumDensity * sqrt(dot(lightVecs[i], lightVecs[i]))));

    color += BlinnPhong(E, normalize(lightVecs[i]), N, lightIntensity, albedo, f0, roughness);
  }
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, 1.);
}
