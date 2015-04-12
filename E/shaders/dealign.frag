//
// Décalle les composantes R et B de part et d'autre
//
// Exemple d'utilisation :
// color = dealign(cMap, gl_TexCoord[0].xy, 0.005);
//
vec4 dealign(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = vec2(intensity, 0);

  float r = texture2D(tex, uv - offset).r;
  float g = texture2D(tex, uv).g;
  float b = texture2D(tex, uv + offset).b;
  float a = texture2D(tex, uv).a;
  return vec4(r, g, b, a);
}
