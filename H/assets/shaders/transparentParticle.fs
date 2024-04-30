#version 330

const int i_NUM_LIGHTS = 5;

uniform float forwardScatteringStrength;
uniform float mediumDensity;
uniform float opacity;
uniform float time;
uniform int lightTypes[i_NUM_LIGHTS];
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform vec3 diffuseColor;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;

in vec2 texCoord;
in vec3 bitangent;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 tangent;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];
in vec4 viewSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec2 p = 2. * texCoord - 1.;
  float d = length(p);
  if (d > 1.)
    discard;

  vec3 T_ = tangent;
  vec3 B_ = bitangent;
  vec3 N_ = cross(T_, B_);

  vec3 E = normalize(viewVec);
  vec3 N = (p.x * T_ + p.y * B_ + sqrt(1. - clamp(d * d, 0., 1.)) * N_) * smoothstep(1.01, 1., d);

  vec3 albedo = diffuseColor;

  vec3 color = vec3(0.);
  float shadowAttenuations[i_NUM_LIGHTS];
  shadowAttenuations[0] = getProjectorLightCone(shadowCoord[0], lightTypes[0], time) * getVSMAttenuation(shadowMaps[0], shadowCoord[0]);
  shadowAttenuations[1] = getProjectorLightCone(shadowCoord[1], lightTypes[1], time) * getVSMAttenuation(shadowMaps[1], shadowCoord[1]);
  shadowAttenuations[2] = getProjectorLightCone(shadowCoord[2], lightTypes[2], time) * getVSMAttenuation(shadowMaps[2], shadowCoord[2]);
  shadowAttenuations[3] = getProjectorLightCone(shadowCoord[3], lightTypes[3], time) * getVSMAttenuation(shadowMaps[3], shadowCoord[3]);
  shadowAttenuations[4] = getProjectorLightCone(shadowCoord[4], lightTypes[4], time) * getVSMAttenuation(shadowMaps[4], shadowCoord[4]);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = shadowAttenuations[i] / (1. + dot(lightVecs[i], lightVecs[i]));
    vec3 lightIntensity = attenuation * (lightColors[i] * getDistanceAbsorption(mediumColor, mediumDensity * sqrt(dot(lightVecs[i], lightVecs[i]))));

    vec3 L = normalize(lightVecs[i]);
    color += albedo * lightIntensity * HenyeyGreenstein(dot(L, -E), forwardScatteringStrength);
  }
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  float thickness = 1. - d * d;
  float density = thickness * thickness * thickness;
  float finalOpacity = opacity * mix(1., density, d);
  fragmentColor = vec4(color, finalOpacity);
}
