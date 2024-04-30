#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform vec3 lightColors[i_NUM_LIGHTS];

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;

out vec4 fragmentColor;

#include "assets/shaders/debug/debugFunctions.fs"
#include "assets/shaders/lightFunctions.fs"

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(-normal);
  vec3 albedo = vec3(1., 0.15, 0.05) * mix(0.8, 1., debugStripes());
  vec3 f0 = vec3(0.02);

  vec3 color = vec3(0.);
  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  }

  fragmentColor = vec4(color, 1.);
}
