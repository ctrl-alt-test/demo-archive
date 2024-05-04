//
// Schlick approximation of Fresnel.
//
// f0 is the RGB reflectivity at normal incidence.
//   0.02 to 0.04 for most materials,
//   0.1 for some cristals
//   0.5 and above for metals
//
vec3 Schlick(vec3 f0, vec3 E, vec3 H)
{
  float x = 1. - clamp(dot(E, H), 0., 1.);
  return x*x*x*x*x * (1. - f0) + f0;
}

// Monochromatic version that handles n1 > n2.
float Schlick(float n1, float n2, vec3 E, vec3 H)
{
  float f0 = (n1 - n2) / (n1 + n2);
  f0 *= f0;
  float cosTheta = clamp(dot(E, H), 0., 1.);
  if (n1 > n2)
  {
    float n = n1 / n2;
    float sqrSinTheta = n * n * (1. - cosTheta * cosTheta);
    if (sqrSinTheta >= 1.)
    {
      // Total reflection.
      return 1.;
    }

    // This is actually not the same theta anymore: replacing theta_i
    // with theta_t.
    cosTheta = sqrt(1. - sqrSinTheta);
  }
  float x = 1. - cosTheta;
  return x*x*x*x*x * (1. - f0) + f0;
}

// Chromatic version that handles n1 > n2.
vec3 Schlick(vec3 n1, vec3 n2, vec3 E, vec3 H)
{
  vec3 f0 = (n1 - n2) / (n1 + n2);
  f0 *= f0;
  vec3 cosTheta = vec3(clamp(dot(E, H), 0., 1.));
  if (n1.r > n2.r)
  {
    vec3 n = n1 / n2;
    vec3 sqrSinTheta = clamp(n * n * (1. - cosTheta * cosTheta), 0., 1.);

    // This is actually not the same theta anymore: replacing theta_i
    // with theta_t.
    cosTheta = sqrt(1. - sqrSinTheta);
  }
  vec3 x = 1. - cosTheta;
  return x*x*x*x*x * (1. - f0) + f0;
}

//
// Blinn-Phong.
//
vec3 BlinnPhong(vec3 E, vec3 L, vec3 N, vec3 lightColor, vec3 albedo, vec3 f0, float roughness)
{
  vec3 H = normalize(E + L);
  float alpha = 1. + 2048. * (1. - roughness)*(1. - roughness);

  vec3 diffuse = albedo;
  vec3 specular = vec3(pow(clamp(dot(H, N), 0., 1.), alpha) * (alpha + 4.) / 8.);
  vec3 fresnel = Schlick(f0, E, H);

  return clamp(dot(L, N), 0., 1.) * lightColor * mix(diffuse, specular, fresnel);
}

//
//
// Reference:
// http://blog.stevemcauley.com/2011/12/03/energy-conserving-wrapped-diffuse/
vec3 WrappedDiffuse(vec3 L, vec3 N, vec3 lightColor, vec3 albedo, float wrap)
{
  return clamp((dot(N, L) + wrap) / ((1 + wrap) * (1 + wrap)), 0., 1.) * lightColor * albedo;
}

//
// Henyey-Greenstein approximation of Mie scattering.
//
// g in [-1 .. 1] determines the relative strength of the forward and backward scattering.
float HenyeyGreenstein(float cosTheta, float g)
{
  return 0.25 * (1. - g*g) / pow(1. + g*g - 2.*g*cosTheta, 1.5);
}

//
// Rayleigh scattering.
//
float Rayleigh(vec3 L, vec3 E)
{
  float cosTheta = dot(L, E);
  return 3./8. * (1. + cosTheta * cosTheta);
}

vec3 getDistanceAbsorption(vec3 color, float d)
{
  if (d <= 0.)
    return vec3(1.);
  return pow(color, vec3(d));
}

float getCaustic(vec2 uv, float time)
{
  float t = time * 1.5;
  vec2 i = uv + vec2(cos(t - uv.x) + sin(t + uv.y),
		     sin(t - uv.y) + cos(t + uv.x));
  return mix(1.5, 0.2, smoothstep(0., 1., 1./length(1. / vec2(sin(i.x+t), cos(i.y+t)))));
}

float getProjectorLightCone(vec4 shadowCoord, int lightType, float time)
{
  vec2 coord = (shadowCoord.xy / shadowCoord.w);
  float d2 = dot(coord, coord);

  float lit = clamp(shadowCoord.z, 0., 1.);
  if (lightType == 0) return lit * smoothstep(1., 0.8, d2);
  if (lightType == 2) return lit * getCaustic(20.*coord, time);

  // Projector.
  float e = clamp(d2, 0., 0.25)/0.25;
  e *= e; e *= e; e *= e;
  float inner = mix(1., 4., e);
  float outer = mix(0.01, 0.25, smoothstep(1., 0.5, d2));
  return mix(outer, inner, smoothstep(0.3, 0.2, d2)) * lit;
}

float getShadowAttenuation(sampler2D shadowMap, vec4 shadowCoord, float bias)
{
  vec3 coord = (shadowCoord.xyz / shadowCoord.w) * 0.5 + 0.5;
  if (shadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    float shadowDist = texture2D(shadowMap, coord.xy).x;
    return float(shadowDist >= coord.z - bias);
  }

  // Outside of the shadow map
  return 1.;
}

float linstep(float vmin, float vmax, float v)
{
  return clamp((v - vmin) / (vmax - vmin), 0., 1.);
}

float ChebyshevUpperBound(vec2 moments, float t)
{
  // One-tailed inequality valid if t > Moments.x.
  float p = float(t <= moments.x);

  // Compute variance.
  float minVariance = 0.000001;
  float variance = max(moments.y - (moments.x * moments.x), minVariance);

  // Compute probabilistic upper bound.
  float d = t - moments.x;
  float pmax = linstep(0.3, 1., variance / (variance + d * d));
  return max(p, pmax);
}

float getVSMAttenuation(sampler2D shadowMap, vec4 shadowCoord)
{
  vec3 coord = (shadowCoord.xyz / shadowCoord.w) * 0.5 + 0.5;
  if (shadowCoord.w > 0. &&
      coord.xy == clamp(coord.xy, 0., 1.))
  {
    // Read the moments from the variance shadow map.
    vec2 moments = texture2D(shadowMap, coord.xy).xy;
    return ChebyshevUpperBound(moments, coord.z);
  }

  // Outside of the shadow map
  return 1.;
}
