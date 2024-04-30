#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform float time;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform vec3 lightColors[i_NUM_LIGHTS];

flat in int instanceId;
in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

vec4 getColor()
{
// Commented out and written with series of if because ShaderMinifier
// stops with a parse error.
/*
  switch (instanceId % 8)
  {
  case 0: return texture2D(texture0, texCoord);
  case 1: return texture2D(texture1, texCoord);
  case 2: return texture2D(texture2, texCoord);
  case 3: return texture2D(texture3, texCoord);
  case 4: return texture2D(texture4, texCoord);
  case 5: return texture2D(texture5, texCoord);
  case 6: return texture2D(texture6, texCoord);
  case 7: return texture2D(texture7, texCoord);
  }
*/
  int textureID = instanceId % 8;
  if (textureID == 0) { return texture2D(texture0, texCoord); }
  if (textureID == 1) { return texture2D(texture1, texCoord); }
  if (textureID == 2) { return texture2D(texture2, texCoord); }
  if (textureID == 3) { return texture2D(texture3, texCoord); }
  if (textureID == 4) { return texture2D(texture4, texCoord); }
  if (textureID == 5) { return texture2D(texture5, texCoord); }
  if (textureID == 6) { return texture2D(texture6, texCoord); }
  return texture2D(texture7, texCoord);
}

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec4 albedo_A = getColor();
  vec3 f0 = vec3(0.02);

  vec3 color = vec3(0.);
  float shadowAttenuations[i_NUM_LIGHTS];
  shadowAttenuations[0] = getProjectorLightCone(shadowCoord[0], 0, time) * getVSMAttenuation(shadowMaps[0], shadowCoord[0]);
  shadowAttenuations[1] = getProjectorLightCone(shadowCoord[1], 0, time) * getVSMAttenuation(shadowMaps[1], shadowCoord[1]);
  shadowAttenuations[2] = getProjectorLightCone(shadowCoord[2], 0, time) * getVSMAttenuation(shadowMaps[2], shadowCoord[2]);
  shadowAttenuations[3] = getProjectorLightCone(shadowCoord[3], 0, time) * getVSMAttenuation(shadowMaps[3], shadowCoord[3]);
  shadowAttenuations[4] = getProjectorLightCone(shadowCoord[4], 0, time) * getVSMAttenuation(shadowMaps[4], shadowCoord[4]);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = shadowAttenuations[i] / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo_A.rgb, f0, roughness);
  }

  fragmentColor = vec4(color, albedo_A.a);
}
