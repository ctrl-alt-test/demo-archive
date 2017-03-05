// Version 1.20 pour les tableaux
#version 120

uniform float offset[5] = float[5](0., 1., 2., 3., 4.);
uniform float weight[5] = float[5](0.2270270270,
				  0.1945945946,
				  0.1216216216,
				  0.0540540541,
				  0.0162162162);

uniform vec2 center;

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

/*
//
// Flou axial
//
// Exemple d'utilisation :
// color = axialBlur(cMap, gl_TexCoord[0].xy, 0.05);
//
vec4 axialBlur(sampler2D tex, vec2 uv, float intensity)
{
  // FIXME : se comporte mal sur les bords
  vec2 dist = uv - center;
  vec2 offset = intensity * vec2(dist.y, -dist.x);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.1;
  for(i = -1.; i <= 1.; i += inc)
  {
    c += 0.5 * inc * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}

//
// Flou radial
//
// Exemple d'utilisation :
// color = radialBlur(cMap, gl_TexCoord[0].xy, 0.05);
//
vec4 radialBlur(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = 2. * intensity * (center - uv);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.05;
  for(i = 0.; i <= 1.; i += inc)
  {
    c += inc * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}

//
// Glow radial
//
// Exemple d'utilisation :
// color = radialGlow(cMap, gl_TexCoord[0].xy, 0.2);
//
vec4 radialGlow(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = 2. * intensity * (center - uv);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.02;
  for(i = 0.; i <= 1.; i += inc)
  {
    vec4 fetch = texture2D(tex, uv + i * offset);
    c += (1.-i) * fetch.rgb * (1. - fetch.a);
  }
  vec4 cur = texture2D(tex, uv);
  return cur + vec4(pow(inc * c, vec3(.5)), cur.a);
}
*/

//
// Flou de mouvement, d'après une vélocity map
//
// Exemple d'utilisation :
// color = motionBlur(cMap, nMap, gl_TexCoord[0].xy, 1.);
//
vec4 motionBlur(sampler2D tex, sampler2D motion, vec2 uv, float intensity)
{
  vec3 speedInfo = texture2D(motion, uv).rgb;
  vec2 speed = (2. * speedInfo.xy - 1.) * pow(speedInfo.z, 2.);
  vec2 offset = intensity * speed;
  vec3 c = vec3(0.);

  float inc = 0.2;
  float weight = 0.;
  for(float i = -1.; i <= 1.; i += inc)
  {
    c += texture2D(tex, clamp(uv + i * offset, vec2(0.), vec2(1., center.y / center.x))).rgb;
    weight += 1.;
  }
  c /= weight;
  return vec4(c, texture2D(tex, uv).a);
}
