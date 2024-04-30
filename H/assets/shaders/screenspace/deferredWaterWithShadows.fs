#version 330

const int i_NUM_LIGHTS = 5;

uniform float lightConeCos[i_NUM_LIGHTS];
uniform float roughness;
uniform float time;
uniform mat4 inverseViewProjectionMatrix;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform sampler2D depthMap;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform samplerCube diffuseEnvMap;
uniform samplerCube specularEnvMap;
uniform vec3 cameraPosition;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 specularColor;
uniform vec4 lightPositions[i_NUM_LIGHTS];

in vec2 texCoord;
//[
layout(location = 0) out vec4 fragmentColor;
//]

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec4 tap = texture2D(texture0, texCoord);
  if (tap.a < 0.5)
    discard;

  float depth = texture2D(depthMap, texCoord).x;
  vec4 worldSpacePosition = inverseViewProjectionMatrix * vec4(texCoord * 2. - 1., depth * 2. - 1., 1.);
  worldSpacePosition /= worldSpacePosition.w;
  vec3 E = normalize(cameraPosition - worldSpacePosition.xyz);
  vec3 N = normalize(tap.rgb * 2. - 1.);

  vec3 screenSpaceNormal = transpose(mat3(inverseViewProjectionMatrix)) * N;
  vec2 refractionOffset = 0.1 * screenSpaceNormal.xy;
  vec2 UV_r = clamp(texCoord - 0.98 * refractionOffset, 0., 1.);
  vec2 UV_g = clamp(texCoord - 1. * refractionOffset, 0., 1.);
  vec2 UV_b = clamp(texCoord - 1.02 * refractionOffset, 0., 1.);

  vec3 albedo =
    texture2D(texture1, UV_r).rgb * vec3(0.75, 0.25, 0.) +
    texture2D(texture1, UV_g).rgb * vec3(0.25, 0.5, 0.25) +
    texture2D(texture1, UV_b).rgb * vec3(0., 0.25, 0.75);
  vec3 f0 = specularColor;

  vec3 fresnel = Schlick(f0, E, N);
  vec3 color = mix(albedo,
		   texture(specularEnvMap, reflect(-E, N)).rgb,
		   fresnel);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    vec4 shadowCoord = modelLightProjectionMatrix[i] * worldSpacePosition;
    float shadowAttenuation = getProjectorLightCone(shadowCoord, 0, time) *
      getShadowAttenuation(shadowMaps[i], shadowCoord, 0.);

    vec3 lightVec = lightPositions[i].xyz - lightPositions[i].w * worldSpacePosition.xyz;
    float attenuation = shadowAttenuation / (1. + dot(lightVec, lightVec));
    color += 10.*BlinnPhong(E, normalize(lightVec), N, attenuation * lightColors[i], vec3(0.), f0, roughness);
  }

  fragmentColor = vec4(color, float(tap.a > 0.5));
}
