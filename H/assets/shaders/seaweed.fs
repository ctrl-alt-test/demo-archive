#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float parallaxScale;
uniform float time;
uniform int lightTypes[i_NUM_LIGHTS];
uniform sampler2D diffuseTexture;
uniform sampler2D normalAndHeightTexture;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform sampler2D specularAndRoughnessTexture;
// uniform samplerCube diffuseEnvMap;
// uniform samplerCube specularEnvMap;
uniform vec3 ambientLight;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 tangent;
in vec3 viewVec;
in vec4 shadowCoord[i_NUM_LIGHTS];
in vec4 viewSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"
#include "assets/shaders/parallaxMappingFunctions.fs"

void main()
{
  vec3 T_ = normalize(tangent);
  vec3 N_ = normalize(normal);
  vec3 B_ = cross(N_, T_);
  mat3 tbn = mat3(T_, B_, N_);

  vec3 E = normalize(viewVec);
  vec2 uv = texCoord;

  vec3 N = tbn * normalize(2. * texture2D(normalAndHeightTexture, uv).xyz - 1.);

  vec4 albedo_A = texture2D(diffuseTexture, uv);
  vec3 f0 = texture2D(specularAndRoughnessTexture, uv).rgb;
  float roughness = texture2D(specularAndRoughnessTexture, uv).a;

  vec3 color = mix(albedo_A.rgb * ambientLight, ambientLight, Schlick(f0, E, N));
  /* mix(albedo_A.rgb * texture(diffuseEnvMap, N).rgb,
     texture(specularEnvMap, reflect(-E, N)).rgb,
     Schlick(f0, E, N)) */

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

    color += BlinnPhong(E, normalize(lightVecs[i]), N, lightIntensity, albedo_A.rgb, f0, roughness);
  }
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, albedo_A.a);
}
