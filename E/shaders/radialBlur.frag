uniform float screenCX;
uniform float screenCY;

//
// Flou radial
//
// Exemple d'utilisation :
// color = radialBlur(cMap, gl_TexCoord[0].xy, 0.05);
//
vec4 radialBlur(sampler2D tex, vec2 uv, float intensity)
{
  vec2 center = vec2(screenCX, screenCY);
  vec2 offset = 2. * intensity * (center - uv);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.05;
  for(i = 0.; i <= 1.; i += inc)
  {
    c += inc * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}

//
// Glow radial
//
// Exemple d'utilisation :
// color = radialGlow(cMap, gl_TexCoord[0].xy, 0.2);
//
vec4 radialGlow(sampler2D tex, vec2 uv, float intensity)
{
  vec2 center = vec2(screenCX, screenCY);
  vec2 offset = 2. * intensity * (center - uv);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.02;
  for(i = 0.; i <= 1.; i += inc)
  {
    vec4 fetch = texture2D(tex, uv + i * offset);
    c += (1.-i) * fetch.rgb * (1. - fetch.a);
  }
  vec4 cur = texture2D(tex, uv);
  return cur + vec4(pow(inc * c, vec3(.5)), cur.a);
}
