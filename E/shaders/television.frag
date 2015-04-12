uniform sampler2D cMap;
uniform int time;

vec4 TVdealign(sampler2D tex, vec2 uv, float intensity);
vec4 addTVNoise(vec4 color, vec2 uv, float intensity, float speed);
vec4 addVignetting(vec4 color, vec2 uv, float radius, float intensity);

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  vec4 color = TVdealign(cMap, uv, 1.);
  color = addTVNoise(color, uv, 0.3, 1. + sin(0.001 * time));
  color = addVignetting(color, uv, 0.5, 0.6);
  gl_FragColor = color;
}
