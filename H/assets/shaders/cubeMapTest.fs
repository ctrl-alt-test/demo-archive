#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform samplerCube texture0;
uniform vec3 lightColors[i_NUM_LIGHTS];

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
  vec3 R = reflect(E, N);
  vec3 albedo = vec3(0.);
  vec3 specularColor = vec3(0.955008, 0.637427, 0.538163); // copper

  vec3 color = vec3(0.);
  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, specularColor, roughness);
  }
  //color += 0.2*texture(texture0, R).rgb;
  color += BlinnPhong(E, -R, N, 0.01 * texture(texture0, R).rgb, albedo, specularColor, roughness);

  fragmentColor = vec4(color, 1.);
}
