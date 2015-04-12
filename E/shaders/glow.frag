// -*- glsl -*-
 
// Inspiration
// http://myheroics.wordpress.com/2008/09/04/glsl-bloom-shader/

uniform sampler2D cMap;
uniform sampler2D sMap;
uniform sampler2D c2Map;

uniform float glowFlash;
uniform float fade;
uniform float dof; // Information de la caméra, entre 0 et 1
uniform float fadeLuminance;
uniform float trans; // transition par la texture

uniform int time;
uniform float screenCX;
uniform float screenCY;

float getDepth(vec2 uv);

vec4 pixelFade(vec4 col)
{
  vec2 texcoord = vec2(gl_TexCoord[0]);
  float n = 100.;
  texcoord.x = float(int(texcoord.x * n)) / n;
  texcoord.y = float(int(texcoord.y * n)) / n;
  vec4 pixels = texture2D(cMap, texcoord);

  float grey = max(pixels.r, max(pixels.g, pixels.b));
  pixels = vec4(grey, grey, grey*0.3, 1.);

  return mix(col, pixels, glowFlash);
}

/*
float rand(vec2 texcoord)
{
  return fract(sin(dot(texcoord, vec2(12.9898,78.233)) + time * 0.0001) * 43758.5453) * 2. - 1.;
}
*/

vec3 rand(vec3 v)
{
  float seed = v.x * v.y * v.z;
  return fract(vec3(sin(seed * 12.9898),
		    sin(seed * 78.233),
		    sin(seed * 91.2228)) * 43758.5453);
}

vec4 vignetting(vec4 col)
{
  vec2 texcoord = vec2(gl_TexCoord[0]);
  vec2 center = vec2(screenCX, screenCY);
  float vi = distance(texcoord, center);
  float m = length(center);
  vi = smoothstep(1., 0.7, vi / m);
  vi = mix(0.5, 1., vi);

  vec3 noisyCol = clamp(0.97 * col.rgb + 0.03 * rand(vec3(texcoord, time * 0.0001)), 0., 1.);
  return vec4(vi * noisyCol, col.a);
}

//glow + dof
vec4 glof()
{
  vec4 gsum = vec4(0);
  vec2 texcoord = vec2(gl_TexCoord[0]);
  float glowMin = glowFlash * 0.18;

  float sumi = 1.; // somme des intensites pour le flou
  vec4 dsum = vec4(0.);
  float dintensity = 0.;
  if (time >= 9800)
  {
    dintensity = abs(getDepth(texcoord) - dof);
    dintensity = pow(dintensity, 1.5); // largeur de la dof
    dintensity *= 1. / 130.; // distance max du flou
  }

  float i, j;
  const float quality = 1. / 3.;
  for(i = -1.; i < 1.01; i += quality)
    {
      for (j = -1.; j < 1.01; j += quality)
        {
          vec2 pos;
          vec4 tex;
          vec2 v = vec2(i, j);
          float coef = 2. - length(v);

          // dof
          pos = texcoord + v * dintensity;
          tex = texture2D(cMap, pos);
          sumi += coef;
          dsum += tex * coef;

          // glow
          pos = texcoord + v * 0.008;
          pos = clamp(pos, 0.001, 0.999);
          tex = texture2D(cMap, pos);
          coef *= 1. - tex.a;
          //glowMin desactive pour le moment
          //coef = clamp(coef, glowMin, 1.);
          gsum += tex * coef;
        }
    }
  return dsum / sumi + gsum * 0.06;
}

vec4 get(vec2 coord)
{
  return coord.x <= 0. ? vec4(0.) : texture2D(cMap, 0.7 * coord);
}

void main()
{
  vec2 texcoord = vec2(gl_TexCoord[0]);

  vec4 col;
  col = texture2D(cMap, texcoord);
  col = glof();
  col = pixelFade(col);

  if (time > 269000 && time < 292000)
    {
      texcoord.x = clamp(1. - texcoord.x, 0.01, 1.);
      vec4 col2 = vec4(max(col.rgb, get(0.7 * texcoord).ggg * 0.25), col.a);
      col2.rgb = max(col2.rgb, get(1.4 * texcoord).ggg * 0.1);
      float t = smoothstep(269000., 270000., float(time));
      col = mix(col, col2, t);
    }
  col = vignetting(col);
  col = mix(vec4(fadeLuminance), col, fade);
  col *= smoothstep(trans-0.03, trans, texture2D(sMap, gl_TexCoord[0].st).x);
  gl_FragColor = col;
}
