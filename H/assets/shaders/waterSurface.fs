#version 330

const int i_NUM_LIGHTS = 5;

uniform float roughness;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 lightColors[i_NUM_LIGHTS];

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 viewVec;
in vec4 clipSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"

// FIXME: many things
// - tweak reflectionPerturbation or expose it as a uniform;
// - compute the coordinates correctly (it's a complete hack now);
// - include the light reflections
//

float reflectionPerturbation = 0.02;

void main()
{
  vec3 E = normalize(viewVec);
  vec3 N = normalize(normal);
  vec4 albedo = vec4(0.01, 0.1, 1., 1.);
  vec3 f0 = vec3(0.022);

  vec2 uv = 0.5 + 0.5 * clipSpacePosition.xy / clipSpacePosition.w;
  vec3 reflection = texture2D(texture0, uv + reflectionPerturbation * N.xy).rgb;
  vec3 refraction = texture2D(texture1, uv + reflectionPerturbation * N.xy).rgb;

  vec3 color = mix(refraction, reflection, Schlick(f0, E, N));
  //for (int i = 0; i < i_NUM_LIGHTS; ++i)
  //{
  //  color += BlinnPhong(E, normalize(lightVecs[i]), N, attenuation * lightColors[i], albedo, f0, roughness);
  //}

  fragmentColor = vec4(color, 1.);
}
