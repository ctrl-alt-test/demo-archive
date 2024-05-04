//
// Different variants of parallax mapping.
//
// The functions take the sampler as a parameter, but all assume the
// height to be stored in W.
//

// vec2 getUVParallaxMapping(sampler2D heightMap, vec2 uv0, vec3 E, float scale)
// {
//   float h0 = texture2D(heightMap, uv0).w;
//   vec2 uv1 = uv0 + scale * h0 * (E.xy / E.z);

//   return uv1;
// }

// vec2 getUVParallaxMappingWithOffsetLimiting(sampler2D heightMap, vec2 uv0, vec3 E, float scale)
// {
//   float h0 = texture2D(heightMap, uv0).w;
//   vec2 uv1 = uv0 + scale * h0 * E.xy;

//   return uv1;
// }

// vec2 getUVParallaxMappingSecant(sampler2D heightMap, vec2 uv0, vec3 E, float scale)
// {
//   float h0 = texture2D(heightMap, uv0).w;
//   vec2 uv1 = uv0 + scale * h0 * (E.xy / E.z);

//   float h1 = texture2D(heightMap, uv1).w;
//   vec2 uv2 = uv1 + scale * (h1 - h0) * (E.xy / E.z);

//   return uv2;
// }

vec2 getUVLinearSearch(sampler2D heightMap, vec2 uv0, vec3 E, float scale)
{
  float maxSteps = 16.;
  float inc = 1. / maxSteps;//mix(maxSteps, 1., E.z * E.z);

  vec2 duv = scale * E.xy / E.z;
  vec2 uv = uv0 + duv;
  float h = texture2D(heightMap, uv).w;

  for (float i = 1.; i >= 0.; i -= inc)
  {
    //cost = (1.-i) * steps/maxSteps;
    vec2 uvi = uv0 + i * duv;
    float hi = texture2D(heightMap, uvi).w;
    if (hi > i)
    {
      float w = (hi - i) / (hi - h + inc);
      return mix(uvi, uv, w);
    }
    uv = uvi;
    h = hi;
  }

  return uv;
}
