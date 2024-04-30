#version 330

const int i_NUM_LIGHTS = 5;

uniform float time;
uniform float roughness;
uniform vec3 lightColors[i_NUM_LIGHTS];

in vec3 vertexCoord;
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
  float t1 = 0.75 + 0.25 * sin(time * 0.001);
  float t2 = 0.75 + 0.25 * cos(time * 0.001);
  vec3 albedo = t1 + t2 * vec3(t1, t2, t1*t2);
  vec3 f0 = vec3(0.02);

  float t = sin(time * 0.001);
  albedo *= t1 * sin(vertexCoord.x*1000) + t2 * cos(vertexCoord.z*1000);

  vec3 color = vec3(0.);
  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  }

  fragmentColor = vec4(color, 1.);
}
