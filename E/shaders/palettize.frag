//
// Réduction des couleurs à une palette 256 naïve
//
// Exemple d'utilisation :
// color = palettize(texture2D(cMap, gl_TexCoord[0].xy).rgb);
//
vec3 palettize(vec3 color)
{
  vec3 nuances = vec3(8., 8., 4.);
  return color - fract(nuances * color) / nuances;
}
