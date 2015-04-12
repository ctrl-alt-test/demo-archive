uniform int time;

vec4 dealign(sampler2D tex, vec2 uv, float intensity);

//
// Décalle les composantes R et B de part et d'autre en suivant un
// motif d'onde
//
// Exemple d'utilisation :
// color = TVdealign(cMap, gl_TexCoord[0].xy, 1.);
//
vec4 TVdealign(sampler2D tex, vec2 uv, float intensity)
{
  float y = gl_TexCoord[0].y;
  float var1 = pow(abs(sin(10. * y + 0.001 * time)), 2.);
  float var2 = 0.75 + 0.25 * sin(500. * y + 0.01 * time);
  float var = 0.002 + 0.0015 * var1 * var2;
  return dealign(tex, uv, var * intensity);
}
