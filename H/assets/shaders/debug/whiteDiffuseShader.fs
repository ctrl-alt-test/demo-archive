#version 330

const int i_NUM_LIGHTS = 5;

uniform float time;
uniform vec3 lightColors[i_NUM_LIGHTS];

in vec3 normal;
in vec3 viewVec;
in vec3 lightVecs[i_NUM_LIGHTS];
out vec4 fragmentColor;

vec3 Lambert(vec3 N, vec3 albedo, vec3 lightColor, vec3 lightVec)
{
  vec3 L = normalize(lightVec);
  float diffuse = clamp(dot(N, L), 0., 1.);

  float attenuation = 1. / (1. + dot(lightVec, lightVec));

  return attenuation * lightColor * (albedo * diffuse);
}

void main()
{
  vec3 N = normalize(normal);
  vec3 V = normalize(viewVec);
  vec4 albedo = vec4(1.);

  vec3 color = vec3(0.);
  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    color += Lambert(N, albedo.rgb, lightColors[i], lightVecs[i]);
  }

  fragmentColor = vec4(color, albedo.a);
}
