//
uniform sampler2D cMap; // color
uniform sampler2D dMap; // speed
uniform sampler2D nMap; // depth

uniform float fade;
uniform float fadeLuminance;
uniform int time;

//[
vec4 addBorder(vec4 color, vec2 uv, float thickness);
vec4 addVignetting(vec4 color, vec2 uv);
vec4 combineGlow(vec4 color, vec2 uv);
vec4 grain(vec2 uv, vec4 color, int period, float freq, float intensity);
vec4 motionBlur(sampler2D tex, sampler2D motion, vec2 uv, float intensity);
//]

void main()
{
  vec2 uv = gl_TexCoord[0].xy;

  vec4 color = motionBlur(cMap, dMap, uv, .5);
  color = combineGlow(color, uv);                // Ajout du glow
  color = grain(uv, color, 60, 2., 0.015);        // Ajout du grain
  color = addVignetting(color, uv);                // Ajout du vignetting
  color = addBorder(color, uv, 0.015);                // Ajout de la bordure
  color = mix(vec4(fadeLuminance), color, fade);
  gl_FragColor = color;
}
