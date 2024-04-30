#version 120

uniform sampler2D tex0; // color
uniform sampler2D tex1; // velocity map
uniform sampler2D tex5; // depth
uniform vec2 center;

uniform float fade;
uniform float fadeLuminance;
uniform int time;

uniform float zNear;
uniform float zFar;

//[
vec3 addGrain(vec3 color, vec2 uv);
vec3 addVignetting(vec3 color, vec2 uv);
vec3 addBorder(vec3 color, vec2 uv);
vec2 applyFishEye(vec2 uv);
vec3 combineLensEffects(vec3 color, vec2 uv);
vec3 motionBlur(sampler2D tex, sampler2D depth, sampler2D velocity, vec2 uv, float intensity);
vec3 motionBlurWithAbherration(sampler2D tex, sampler2D depth, sampler2D velocity, vec2 uv, float intensity, vec2 aberration);
vec3 toneMapping(vec3 color);
//]

void main()
{
  vec2 uv = gl_TexCoord[0].xy;
  vec2 uvLens = applyFishEye(uv);

  vec2 coord = uv - center;
  vec2 aberration = coord * 0.003;

  vec3 color = motionBlurWithAbherration(tex0, tex5, tex1, uvLens, .5, aberration);
  color = combineLensEffects(color, uvLens);     // Ajout des effets de lentille

  color = addGrain(color, uv);                   // Ajout du grain
  color = addVignetting(color, uvLens);          // Ajout du vignetting
  color = toneMapping(color);

  color = addBorder(color, uv);                  // Ajout de la bordure
  color = mix(vec3(fadeLuminance), color, fade);

  gl_FragColor = vec4(color, 1.);
}
