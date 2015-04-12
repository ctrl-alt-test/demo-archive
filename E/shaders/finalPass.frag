//
uniform sampler2D cMap; // color
uniform sampler2D nMap; // z Buffer
uniform sampler2D sMap; // speed

uniform float fade;
uniform float fadeLuminance;
uniform int time;

vec4 motionBlur(sampler2D tex, sampler2D motion, vec2 uv, float intensity);
vec4 combineGlow(vec4 color, vec2 uv);
vec4 filter(vec4 color, vec3 low, vec3 hig, vec3 gamma);
vec4 addRegularNoise(vec4 color, vec2 uv, float intensity);
vec4 addVignetting(vec4 color, vec2 uv, float radius, float intensity);
vec4 godrays(vec2 uv);

void main()
{
  vec2 uv = gl_TexCoord[0].xy;

  vec4 color = motionBlur(cMap, sMap, uv, .5);
  color = combineGlow(color, uv);                  // Ajout du glow
  color = addRegularNoise(color, uv, 0.03);        // Ajout du bruit
  color = addVignetting(color, uv, 0.3, 0.35);     // Ajout du vignetting
  color = mix(vec4(fadeLuminance), color, fade);

  gl_FragColor = color;
}
