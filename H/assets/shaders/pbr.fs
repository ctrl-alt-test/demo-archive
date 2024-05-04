#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float parallaxScale;
uniform sampler2D diffuseTexture;
uniform sampler2D normalAndHeightTexture;
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
  vec3 Etan = E * tbn;
  vec2 uv = getUVLinearSearch(normalAndHeightTexture, texCoord, Etan, parallaxScale);

  vec3 N = tbn * normalize(2. * texture2D(normalAndHeightTexture, uv).xyz - 1.);

  vec4 albedo_A = texture2D(diffuseTexture, uv);
  vec3 f0 = texture2D(specularAndRoughnessTexture, uv).rgb;
  float roughness = texture2D(specularAndRoughnessTexture, uv).a;

  vec3 color = mix(albedo_A.rgb * ambientLight, ambientLight, Schlick(f0, E, N));
  /* mix(albedo_A.rgb * texture(diffuseEnvMap, N).rgb,
     texture(specularEnvMap, reflect(-E, N)).rgb,
     Schlick(f0, E, N)*/

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    float attenuation = 1. / (1. + dot(lightVecs[i], lightVecs[i]));
    vec3 lightIntensity = attenuation * (lightColors[i] * getDistanceAbsorption(mediumColor, mediumDensity * sqrt(dot(lightVecs[i], lightVecs[i]))));

    color += BlinnPhong(E, normalize(lightVecs[i]), N, lightIntensity, albedo_A.rgb, f0, roughness);
  }
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, albedo_A.a);
}
