#version 330

const int i_NUM_LIGHTS = 5;

uniform float aboveWater;
uniform float mediumDensity;
uniform float time;
uniform float zFar;
uniform float zNear;
uniform sampler2D randomTexture;
uniform sampler2D reflection;
uniform sampler2D reflectionDepth;
uniform sampler2D refraction;
uniform sampler2D refractionDepth;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;

in vec2 texCoord;
in vec3 lightVecs[i_NUM_LIGHTS];
in vec3 normal;
in vec3 tangent;
in vec3 viewSpaceNormal;
in vec3 viewSpaceTangent;
in vec3 viewVec;
in vec4 clipSpacePosition;
in vec4 viewSpacePosition;
in vec4 worldSpacePosition;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"
#include "assets/shaders/randFunctions.fs"
#include "assets/shaders/waterFunctions.fs"

float getWorldDepth(float z)
{
  return (2.0 * zNear) / (zNear + mix(zFar, zNear, z));
}

void main()
{
  vec3 T_ = normalize(tangent);
  vec3 N_ = -normalize(normal);
  vec3 B_ = cross(N_, T_);
  mat3 tbn = mat3(T_, B_, N_);

  vec3 vsT_ = normalize(viewSpaceTangent);
  vec3 vsN_ = -normalize(viewSpaceNormal);
  vec3 vsB_ = cross(vsN_, vsT_);
  mat3 vstbn = mat3(vsT_, vsB_, vsN_);

  vec2 uv = worldSpacePosition.xz / worldSpacePosition.w;
  vec3 tanSpaceNormal = vec3(0., 0., 1.);

  // The usual waves (small ones; the larger ones are done in the vertex shader).
  tanSpaceNormal.xy += 0.7 * waves(uv, 0.5*time, randomTexture);

  tanSpaceNormal = normalize(tanSpaceNormal);
  vec3 vsN = vstbn * tanSpaceNormal;
  vec3 N = tbn * tanSpaceNormal * mix(1., -1., aboveWater);

  vec3 E = normalize(viewVec);

  uv = 0.5 + 0.5 * clipSpacePosition.xy / clipSpacePosition.w;
  vec2 intensity = vec2(9./16., 1.) * 0.06;

  // This part tries to limit the maximum offset based on the distance
  // from the water surface.
  float distanceToReflection = getWorldDepth(texture2D(reflectionDepth, uv).r) - getWorldDepth(clipSpacePosition.z/clipSpacePosition.w * 0.5 + 0.5);
  float distanceToRefraction = getWorldDepth(texture2D(refractionDepth, uv).r) - getWorldDepth(clipSpacePosition.z/clipSpacePosition.w * 0.5 + 0.5);
  float reflMaxOffset = smoothstep(0., 0.01, distanceToReflection);
  float refrMaxOffset = smoothstep(0., 0.01, distanceToRefraction);

  vec3 refl = texture2D(reflection, clamp(uv + reflMaxOffset * intensity * vsN.xy, 0., 1.)).rgb;
  vec3 refr = texture2D(refraction, clamp(uv + refrMaxOffset * intensity * vsN.xy, 0., 1.)).rgb;
  float horizontal = 1.-E.y*E.y;
  horizontal *= horizontal;
  horizontal *= horizontal;
  horizontal *= horizontal;
  horizontal *= horizontal;
  float waterBrightness = horizontal * (1. - N.y * N.y) * smoothstep(-2., 2., worldSpacePosition.y) * aboveWater;
  vec3 diffuse = refr + vec3(waterBrightness) * vec3(0.15, 0.75, 1.);

  vec3 n1 = mix(vec3(1.325, 1.33, 1.335), vec3(1.), aboveWater);
  vec3 n2 = mix(vec3(1.325, 1.33, 1.335), vec3(1.), 1. - aboveWater);
  vec3 fresnel = Schlick(n1, n2, E, normalize(N+aboveWater*vec3(0., 1., 0.)));
  vec3 color = mix(diffuse, refl, fresnel);
  color *= getDistanceAbsorption(mediumColor, mediumDensity * abs(viewSpacePosition.z/viewSpacePosition.w));

  fragmentColor = vec4(color, 1.);
}
