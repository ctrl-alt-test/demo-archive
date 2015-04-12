// Version 1.20 pour les tableaux
#version 120

uniform float offset[5] = float[](0., 1., 2., 3., 4.);
uniform float weight[5] = float[](0.2270270270,
				  0.1945945946,
				  0.1216216216,
				  0.0540540541,
				  0.0162162162);

const float glowCoef = 0.4;

//
// Flou gaussien vertical
//
// Exemple d'utilisation :
// color = verticalGaussianBlur(cMap, gl_TexCoord[0].xy, 0.005);
//
vec4 verticalGaussianBlur(sampler2D tex, vec2 uv, float intensity)
{
  vec4 c = texture2D(tex, uv) * weight[0];
  vec2 offs = vec2(0.);
  for (int i = 1; i < 5; ++i)
  {
    offs.y = intensity * offset[i];
    c += texture2D(tex, uv + offs) * weight[i];
    c += texture2D(tex, uv - offs) * weight[i];
  }
  return c;
}

//
// Flou gaussien horizontal
//
vec4 horizontalGaussianBlur(sampler2D tex, vec2 uv, float intensity)
{
  vec4 c = texture2D(tex, uv) * weight[0];
  vec2 offs = vec2(0.);
  for (int i = 1; i < 5; ++i)
  {
    offs.x = intensity * offset[i];
    c += texture2D(tex, uv + offs) * weight[i];
    c += texture2D(tex, uv - offs) * weight[i];
  }
  return c;
}

//
// Glow gaussien vertical
//
// Exemple d'utilisation :
// color = verticalGlow(cMap, uv, 0.003);
//
vec4 verticalGlow(sampler2D tex, vec2 uv, float intensity)
{
  vec4 c;
  vec4 sum = vec4(0.);
  vec2 offs = vec2(0.);
  for (int i = 1; i < 5; ++i)
  {
    offs.y = intensity * offset[i];
    c = texture2D(tex, uv + offs);
    sum += c * weight[i] * (1. - c.a);
    c = texture2D(tex, uv - offs);
    sum += c * weight[i] * (1. - c.a);
  }
  c = texture2D(tex, uv);
  sum *= glowCoef;
  return vec4((c + sum).rgb, c.a - sum.a);
}

//
// Glow gaussien horizontal
//
vec4 horizontalGlow(sampler2D tex, vec2 uv, float intensity)
{
  vec4 c;
  vec4 sum = vec4(0.);
  vec2 offs = vec2(0.);
  for (int i = 1; i < 5; ++i)
  {
    offs.x = intensity * offset[i];
    c = texture2D(tex, uv + offs);
    sum += c * weight[i] * (1. - c.a);
    c = texture2D(tex, uv - offs);
    sum += c * weight[i] * (1. - c.a);
  }
  c = texture2D(tex, uv);
  sum *= glowCoef;
  return vec4((c + sum).rgb, c.a - sum.a);
}
