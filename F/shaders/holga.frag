uniform sampler2D cMap;
uniform vec2 center;

//[
vec4 dispersion(sampler2D tex, vec2 uv, float intensity);
vec4 addVignetting(vec4 color, vec2 uv, float radius, float intensity);
//]

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  float dist = distance(uv, center);
  gl_FragColor = addVignetting(dispersion(cMap, uv, 0.06 * dist), uv, 0.5, 0.7);
}
