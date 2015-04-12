uniform float screenCX;
uniform float screenCY;

//
// Assombrit le pourtour de l'image
//
// Exemple d'utilisation :
// color = addVignetting(color, 0.5, 0.6);
//
vec4 addVignetting(vec4 color, vec2 uv, float radius, float intensity)
{
  vec2 center = vec2(screenCX, screenCY);
  float vi = distance(uv, center) / length(center);
  vi = mix(1. - intensity, 1., smoothstep(1., radius, vi));

  return vec4(vi * color.rgb, color.a);
}
