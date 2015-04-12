uniform int time;

// Fonction rand référence
float rand(vec2 v)
{
  return fract(sin(dot(v, vec2(12.9898,78.233))) * 43758.5453);
}

//
// Applique un décalage horizontal aléatoire à chaque bloc de lignes
//
// Exemple d'utilisation :
// color = hcryptic(cMap, gl_TexCoord[0].xy, 0.05, 0.001);
//
vec4 hcryptic(sampler2D tex, vec2 uv, float intensity, float resolution)
{
  vec2 offset = vec2(intensity * rand(vec2(time, uv.y)), 0);
  return texture2D(tex, uv + offset);
}
