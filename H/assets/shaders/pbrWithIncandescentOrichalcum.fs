#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float parallaxScale;
uniform float time;
uniform int lightTypes[i_NUM_LIGHTS];
uniform sampler2D diffuseTexture;
uniform sampler2D normalAndHeightTexture;
uniform sampler2D randomTexture;
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
in vec4 objectSpacePosition;
in vec4 viewSpacePosition;
in vec4 worldSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"
#include "assets/shaders/parallaxMappingFunctions.fs"
#include "assets/shaders/randFunctions.fs"

vec3 getFakeBlackBodyRadiation(float intensity)
{
  return vec3(1., 0.028, 0.002) * (exp(intensity) - 1.);
}

void main()
{
  vec3 T_ = normalize(tangent);
  vec3 N_ = normalize(normal);
  vec3 B_ = cross(N_, T_);
  mat3 tbn = mat3(T_, B_, N_);

  vec3 E = normalize(viewVec);
  vec3 Etan = E * tbn;
  vec2 uv = getUVLinearSearch(normalAndHeightTexture, texCoord, Etan, parallaxScale);

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


  float t = clamp((time - 10.) / 12., 0., 1.);
  float progress = 1.1 * t - 0.1 * hashTex(randomTexture, worldSpacePosition.xz / 25.6);
  float intensity = smoothstep(1., 0., worldSpacePosition.y - 32. * progress) * t*t *
    smoothstep(40.,39., time); // Hack: turn off orichalcum before the emergence shots.

  // Incandescent orichalcum:
  if (f0.r > max(0.3, mix(1., 0.3, intensity)))
  {
    float d = length(vec3(36.5, 10., 71.) - objectSpacePosition.xyz);
    float wave = abs(fract(d - time) * 2. - 1.);
    wave = mix(wave * wave, smoothstep(3.5, 18., objectSpacePosition.y), 0.8);

    float noise = valueNoiseTex(randomTexture, 50. * worldSpacePosition.xyz/worldSpacePosition.w - 2.*vec3(0., time, 0.)) * 2. - 1.;
    float heat = intensity * mix(abs(fract(noise + 1.5 * time) * 2. - 1.), wave, 0.6);
    color += getFakeBlackBodyRadiation(3. * heat);
  }

  fragmentColor = vec4(color, albedo_A.a);
}
