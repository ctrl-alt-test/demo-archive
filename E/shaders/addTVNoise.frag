uniform int time;

// FIXME : si possible factoriser le rand
vec3 tvRand(vec3 v)
{
  float seed = 0.0005 * v.x + v.y + v.z;
  return fract(vec3(sin(seed * 12.9898),
		    sin(seed * 78.233),
		    sin(seed * 91.2228)) * 43758.5453);
}

//
// Ajoute un bruit qui suit des scanlines, type antenne télé mal réglée
//
// Exemple d'utilisation :
// color = addTVNoise(color, uv, 0.4, 1. + sin(0.001 * time));
//
vec4 addTVNoise(vec4 color, vec2 uv, float intensity, float speed)
{
  vec3 noise = tvRand(vec3(uv, time * 0.0001 * speed));
  return vec4(mix(color.rgb, noise, intensity), color.a);
}
