#version 330

const int i_NUM_LIGHTS = 5;

uniform float forwardScatteringStrength;
uniform float lightConeCos[i_NUM_LIGHTS];
uniform float mediumDensity;
uniform float scatteringIntensity;
uniform float time;
uniform int lightTypes[i_NUM_LIGHTS];
uniform int marchingSteps;
uniform mat4 inverseViewProjectionMatrix;
uniform mat4 modelLightProjectionMatrix[i_NUM_LIGHTS];
uniform sampler2D depthMap;
uniform sampler2D randomTexture;
uniform sampler2D shadowMaps[i_NUM_LIGHTS];
uniform vec2 resolution;
uniform vec3 cameraPosition;
uniform vec3 lightColors[i_NUM_LIGHTS];
uniform vec3 mediumColor;
uniform vec4 lightPositions[i_NUM_LIGHTS];

in vec2 texCoord;

out vec4 fragmentColor;

#include "assets/shaders/lightFunctions.fs"
#include "assets/shaders/randFunctions.fs"

struct Ray
{
  vec3 o;     // origin
  vec3 d;     // direction
};
struct Cone
{
  float cosa; // half cone angle
  vec3 c;     // tip position
  vec3 v;     // axis
};

//
// Returns the number of solutions (0, 1 or 2) to the intersection
// between a ray and a cone. t1 and t2 are the solutions.
//
int intersectCone(Cone s, Ray r, out float t1, out float t2)
{
  vec3 co = r.o - s.c;

  float a = dot(r.d,s.v)*dot(r.d,s.v) - s.cosa*s.cosa;
  float b = 2. * (dot(r.d,s.v)*dot(co,s.v) - dot(r.d,co)*s.cosa*s.cosa);
  float c = dot(co,s.v)*dot(co,s.v) - dot(co,co)*s.cosa*s.cosa;

  float det = b*b - 4.*a*c;
  if (det < 0.) return 0;

  det = sqrt(det);
  t1 = (-b - det) / (2. * a);
  bool t1Valid = (dot(r.o + t1*r.d - s.c, s.v) >= 0.);

  t2 = (-b + det) / (2. * a);
  bool t2Valid = (dot(r.o + t2*r.d - s.c, s.v) >= 0.);

  if (!t1Valid && !t2Valid) return 0;
  if (det > 0. && t1Valid && t2Valid)
  {
    if (t1 < 0. && t2 < 0.)
      return 0;
    return 2;
  }

  bool oIsInside = (dot(co,s.v) >= length(co)*s.cosa);
  float t = (t1Valid ? t1 : t2);
  if (!oIsInside && t < 0.) return 0;

  t1 = (oIsInside ? 0. : t);
  t2 = ((t >= 0.) == oIsInside ? t : 1000000.);

  return 1;
}

vec3 march(vec3 startPosition, vec3 endPosition, float offset)
{
  vec3 E = normalize(startPosition - endPosition);
  vec3 color = vec3(0.);

  Ray ray = Ray(startPosition, normalize(endPosition - startPosition));
  float maxt = dot(ray.d, endPosition - startPosition);

  for (int i = 0; i < i_NUM_LIGHTS; ++i)
  {
    if (dot(lightColors[i], lightColors[i]) == 0.) continue;

    vec3 lightPosition = lightPositions[i].xyz;
    vec3 lightDirection = normalize(transpose(modelLightProjectionMatrix[i])[2].xyz);
    Cone cone = Cone(lightConeCos[i], lightPosition, lightDirection);

    float t1 = 0.;
    float t2 = maxt;
    int hits = 2;
    if (lightConeCos[i] > 0.)
    {
      hits = intersectCone(cone, ray, t2, t1);
    }

    if (hits > 0)
    {
      float startt = min(max(0., min(t1, t2)), maxt);
      float endt = min(max(t1, t2), maxt);

      // FIXME: this is a hack to avoid infinite distances, but
      // ideally we want a truncated cone.
      endt = min(endt, startt + 80.);

      if (startt == endt) continue;

      vec4 startPositionLightSpace = modelLightProjectionMatrix[i] * vec4(ray.o + ray.d * startt , 1.);
      vec4 endPositionLightSpace = modelLightProjectionMatrix[i] * vec4(ray.o + ray.d * endt, 1.);

      float intensity = (endt - startt) / marchingSteps;
      for (float j = offset; j < marchingSteps; ++j)
      {
	float x = j / marchingSteps;
	vec3 position = ray.o + ray.d * mix(endt, startt, x);
	float cameraDistance = length(ray.d * mix(endt, startt, x));

	vec4 shadowCoord = mix(endPositionLightSpace, startPositionLightSpace, x);
	float shadowAttenuation = getProjectorLightCone(shadowCoord, lightTypes[i], time) *
	  getShadowAttenuation(shadowMaps[i], shadowCoord, 0.);

	vec3 lightVec = lightPosition - position;
	float lightDistance = sqrt(dot(lightVec, lightVec));
	float attenuation = shadowAttenuation / (1. + dot(lightVec, lightVec));

	vec3 lightIntensity = intensity * attenuation *
	  (lightColors[i] * getDistanceAbsorption(mediumColor, mediumDensity * lightDistance));

	vec3 L = lightVec / lightDistance;
	color += (lightIntensity * HenyeyGreenstein(dot(L, -E), forwardScatteringStrength)) *
	  getDistanceAbsorption(mediumColor, mediumDensity * cameraDistance);
      }
    }
  }
  return color;
}

void main()
{
  vec4 albedo_A = vec4(1., 0.5, 0., 0.05);

  float depth = texture2D(depthMap, texCoord).x;
  vec4 nearPosWorldSpace = inverseViewProjectionMatrix * vec4(texCoord * 2. - 1., -1., 1.);
  vec4 farPosWorldSpace = inverseViewProjectionMatrix * vec4(texCoord * 2. - 1., depth * 2. - 1., 1.);
  nearPosWorldSpace /= nearPosWorldSpace.w;
  farPosWorldSpace /= farPosWorldSpace.w;

  float h = hashTex(randomTexture, resolution * texCoord);
  vec3 color = scatteringIntensity * march(nearPosWorldSpace.xyz, farPosWorldSpace.xyz, h);
  fragmentColor = vec4(color, albedo_A.a);
}
