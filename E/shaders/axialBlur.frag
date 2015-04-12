uniform float screenCX;
uniform float screenCY;

//
// Flou radial
//
// Exemple d'utilisation :
// color = axialBlur(cMap, gl_TexCoord[0].xy, 0.05);
//
vec4 axialBlur(sampler2D tex, vec2 uv, float intensity)
{
  // FIXME : se comporte mal sur les bords
  vec2 center = vec2(screenCX, screenCY);
  vec2 dist = uv - center;
  vec2 offset = intensity * vec2(dist.y, -dist.x);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.1;
  for(i = -1.; i <= 1.; i += inc)
  {
    c += 0.5 * inc * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}
