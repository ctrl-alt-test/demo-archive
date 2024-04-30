#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform float time;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform samplerCube diffuseEnvMap;
uniform samplerCube specularEnvMap;
uniform vec3 diffuseColor;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 specularColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec3 albedo = diffuseColor;
  vec3 f0 = specularColor;

  vec3 fresnel = Schlick(f0, E, N);
  vec3 color = mix(albedo * texture(diffuseEnvMap, N).rgb,
		   texture(specularEnvMap, reflect(-E, N)).rgb,
		   fresnel);
  float shadowAttenuations[i_NUM_LIGHTS];
  shadowAttenuations[0] = getProjectorLightCone(shadowCoord[0], 0, time) * getVSMAttenuation(shadowMaps[0], shadowCoord[0]);
  shadowAttenuations[1] = getProjectorLightCone(shadowCoord[1], 0, time) * getVSMAttenuation(shadowMaps[1], shadowCoord[1]);
  shadowAttenuations[2] = getProjectorLightCone(shadowCoord[2], 0, time) * getVSMAttenuation(shadowMaps[2], shadowCoord[2]);
  shadowAttenuations[3] = getProjectorLightCone(shadowCoord[3], 0, time) * getVSMAttenuation(shadowMaps[3], shadowCoord[3]);
  shadowAttenuations[4] = getProjectorLightCone(shadowCoord[4], 0, time) * getVSMAttenuation(shadowMaps[4], shadowCoord[4]);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = shadowAttenuations[i] / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  }

  fragmentColor = vec4(color, 1.);
}
