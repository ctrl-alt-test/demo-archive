#version 330

const int i_NUM_LIGHTS = 5;

uniform float aboveWater;
uniform float mediumDensity;
uniform float splashIntensity;
uniform float splashRadius;
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

vec2 segment(vec2 p, vec2 a, vec2 b)
{
  vec2 ap = p - a;
  vec2 ab = b - a;
  float h = clamp(dot(ap, ab) / dot(ab, ab), 0., 1.);
  return vec2(length(ap - ab * h), h);
}

vec2 boatRipples(vec2 uv, float time)
{
  vec2 a = vec2(0., -12.);
  vec2 b = vec2(0., 20.);
  float f = 10.;
  vec2 d = segment(uv, a, b);
  vec2 v = normalize(mix(a, b, d.y) - uv);
  float h = 1e-3;
  float d1 = d.x - h;
  float d2 = d.x + h;
  float p1 = sin(f * (d1 - 3. * time)) / f / d1;
  float p2 = sin(f * (d2 - 3. * time)) / f / d2;
  vec2 n = normalize(vec2(p2 - p1, 2. * h));

  return v * n.x;
}

vec2 splashRipples(vec2 uv)
{
  float splashStart = 22.1;
  float splashEnd = 23.;
  float splash = smoothstep(splashStart, splashStart+0.3, time) * smoothstep(splashEnd, splashEnd-0.6, time);
  if (splash <= 0.)
    return vec2(0.);

  vec2 a = vec2(0., 2.25);
  vec2 d = segment(uv, a, -a);
  return splash *
    smoothstep(2., 1.9, d.x) *
    smoothstep(0.4, 0.6, d.x) *
    normalize(uv - mix(a, -a, d.y)) *
    pow(mix(sin(50. * d.x), sin(40. * d.y), 0.3) * 0.5 + 0.5, 2.);
}

// Maximum number of cells a ripple can cross.
#define MAX_RADIUS 3

vec2 splashFalloutRipples(vec2 uv, float time)
{
  float fallout = splashIntensity * smoothstep(splashRadius, 0., length(uv));
  if (fallout <= 0.)
    return vec2(0.);

  vec2 p0 = floor(uv);

  vec2 ripple = vec2(0.);
  for (int j = -MAX_RADIUS; j <= MAX_RADIUS; ++j)
  {
    for (int i = -MAX_RADIUS; i <= MAX_RADIUS; ++i)
    {
      vec2 pi = p0 + vec2(i, j);
      vec2 p = pi + hashTex(randomTexture, pi);

      float t = fract(time + hashTex(randomTexture, pi));
      vec2 v = normalize(p - uv);
      float d = length(p - uv) - (float(MAX_RADIUS) + 0.5) * t;
      float amplitude = smoothstep(-0.6, -0.3, d) * smoothstep(0., -0.3, d);
      float f = 32.;

      if (amplitude > 0.)
      {
	float h = 1e-3;
	float d1 = d - h;
	float d2 = d + h;
	float p1 = sin(f * d1) / f * (1. - t);
	float p2 = sin(f * d2) / f * (1. - t);
	vec2 n = normalize(vec2(p2 - p1, 2. * h));
	ripple += v * n.x;
      }
    }
  }
  return fallout * ripple;
}

#define PI 3.1415926535

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
  tanSpaceNormal.xy += 0.6 * waves(uv, 0.5*time, randomTexture);

  // The ripples around the boat.
  tanSpaceNormal.xy += 0.5 * boatRipples(uv, time / 5.f);

  // The splash.
  tanSpaceNormal.xy += splashRipples(texCoord);

  // The circles due to the fallout from the splash.
  tanSpaceNormal.xy += splashFalloutRipples(texCoord, time / 5.f);

  tanSpaceNormal = normalize(tanSpaceNormal);
  vec3 vsN = vstbn * tanSpaceNormal;
  vec3 N = tbn * tanSpaceNormal * mix(1., -1., aboveWater);

  vec3 E = normalize(viewVec);

  uv = 0.5 + 0.5 * clipSpacePosition.xy / clipSpacePosition.w;
  vec2 intensity = vec2(9./16., 1.) * 0.08;

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
