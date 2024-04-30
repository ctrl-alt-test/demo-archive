#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform sampler2D diffuseTexture;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 specularColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec3 albedo = texture2D(diffuseTexture, texCoord.xy).rgb;
  vec3 f0 = specularColor;

  vec3 color = vec3(0.);
  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  }

  fragmentColor = vec4(color, 1.);
}
