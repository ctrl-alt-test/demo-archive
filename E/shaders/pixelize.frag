//
// Pixelisation
//
// Exemple d'utilisation :
// color = pixelize(cMap, gl_TexCoord[0].xy, 1024. * (1. - abs(sin(0.001 * time))));
//
vec4 pixelize(sampler2D tex, vec2 uv, float resolution)
{
  vec2 coord = floor(resolution * uv) / resolution;
  return texture2D(tex, coord);
}
