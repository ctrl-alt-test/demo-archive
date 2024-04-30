#version 330

const int i_NUM_LIGHTS = 5;

uniform float mediumDensity;
uniform float parallaxScale;
uniform float time;
uniform sampler2D randomTexture;
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
#include "assets/shaders/randFunctions.fs"
#include "assets/shaders/waterFunctions.fs"

void main()
{
  vec3 T_ = normalize(tangent);
  vec3 N_ = normalize(normal);
  vec3 B_ = cross(N_, T_);
  mat3 tbn = mat3(T_, B_, N_);

  vec3 E = normalize(viewVec);
  vec2 uv = texCoord;


  vec3 tanSpaceNormal = vec3(0., 0., 1.);
  tanSpaceNormal.xy += 0.1 * waves(uv, 0.25*time, randomTexture);
  tanSpaceNormal = normalize(tanSpaceNormal);

  vec3 N = tbn * tanSpaceNormal;


  vec3 deepBlue = vec3(0., 0.22, 0.3);
  vec3 lightBlue = vec3(0., 0.15, 0.15);
  vec3 foam = vec3(0.75, 0.98, 1.);

  vec2 p = vec2(350., 40.) - texCoord;
  float test = pow(smoothstep(650., 450., length(p) + valueNoiseTex(randomTexture, vec3(texCoord, texCoord.x + time))), 4.);

  vec4 albedo_A = vec4(mix(deepBlue, lightBlue, pow(test, 0.5)), 1.);
  vec3 f0 = vec3(0.02);

  float roughness = 0.;//mix(0., 1., hashTex(randomTexture, 10.*texCoord));

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
