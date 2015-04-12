uniform int time;

/*
** Fonction rand référence
**
float rand(vec2 v)
{
  return fract(sin(dot(v, vec2(12.9898,78.233))) * 43758.5453);
}
*/

vec3 rand(vec3 v)
{
  // FIXME : vérifier que ça ne fait pas de motif
  float seed = dot(v.xyz, v.yzx);
  return fract(43758.5453 * sin(seed * vec3(12.9898, 78.233, 91.2228)));
}

//
// Ajoute un bruit homogène, type bruit de capteur
//
// Exemple d'utilisation :
// color = addRegularNoise(color, gl_TexCoord[0].xy, 0.03);
//
vec4 addRegularNoise(vec4 color, vec2 uv, float intensity)
{
  vec3 noise = rand(vec3(uv, float(time) * 0.0002));
  return vec4(mix(color.rgb, noise, intensity), color.a);
}
