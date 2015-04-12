//
// Dispersion chromatique
//
// Exemple d'utilisation :
// color = dispersion(cMap, gl_TexCoord[0].xy, 0.02);
//
vec4 dispersion(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = vec2(intensity, 0);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.1; // FIXME: reste à rendre cette valeur dynamique
  for(i = -1.; i <= 1.; i += inc)
  {
    float rb = .5 + i * .5;
    vec3 w = vec3(rb, 1. - abs(i), 1. - rb);
    c += inc * w * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}
