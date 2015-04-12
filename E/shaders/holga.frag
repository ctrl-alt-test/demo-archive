uniform sampler2D cMap;
uniform float screenCX;
uniform float screenCY;

vec4 dispersion(sampler2D tex, vec2 uv, float intensity);
vec4 addVignetting(vec4 color, vec2 uv, float radius, float intensity);

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  vec2 center = vec2(screenCX, screenCY);
  float dist = distance(uv, center);
  gl_FragColor = addVignetting(dispersion(cMap, uv, 0.06 * dist), uv, 0.5, 0.7);
}
