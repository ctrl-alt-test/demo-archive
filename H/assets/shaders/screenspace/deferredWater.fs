#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform mat4 inverseViewProjectionMatrix;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform sampler2D depthMap;
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
  float depth = texture2D(depthMap, texCoord).x;
  vec4 worldSpacePosition = inverseViewProjectionMatrix * vec4(texCoord * 2. - 1., depth * 2. - 1., 1.);
  worldSpacePosition /= worldSpacePosition.w;
  vec3 E = normalize(cameraPosition - worldSpacePosition.xyz);

  vec4 tap = texture2D(texture0, texCoord);
  vec3 N = normalize(tap.rgb * 2. - 1.);

  vec3 screenSpaceNormal = transpose(mat3(inverseViewProjectionMatrix)) * N;
  vec2 refractionOffset = 0.1 * screenSpaceNormal.xy;
  vec2 UV_r = clamp(texCoord - 0.95 * refractionOffset, 0., 1.);
  vec2 UV_g = clamp(texCoord - 1. * refractionOffset, 0., 1.);
  vec2 UV_b = clamp(texCoord - 1.05 * refractionOffset, 0., 1.);

  vec3 albedo = vec3(texture2D(texture1, UV_r).r,
		     texture2D(texture1, UV_g).g,
		     texture2D(texture1, UV_b).b);
  vec3 f0 = specularColor;

  vec3 fresnel = Schlick(f0, E, N);
  vec3 color = mix(albedo,
		   texture(specularEnvMap, reflect(-E, N)).rgb,
		   fresnel);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    vec3 lightVec = lightPositions[i].xyz - lightPositions[i].w * worldSpacePosition.xyz;
    float attenuation = 1. / (1. + dot(lightVec, lightVec));
    color += BlinnPhong(E, normalize(lightVec), N, attenuation * lightColors[i], vec3(0.), f0, roughness);
  }

  fragmentColor = vec4(color, float(tap.a > 0.5));
}
