// -*- glsl -*-

// http://www.ozone3d.net/tutorials/image_filtering_p2.php

float kernel[25] = float[25](1., 4., 7., 4., 1.,
			     4., 16., 26., 16., 4.,
			     7., 26., 41., 26., 7.,
			     4., 16., 26., 16., 4.,
			     1., 4., 7., 4., 1.);

varying vec2 texCoord;

uniform sampler2D cMap;
uniform sampler2D nMap;
uniform sampler2D sMap;

uniform int time;
uniform float dof;
uniform float fade;
uniform float fadeLuminance;
uniform float trans; // transition par la texture

// recupere le z-buffer correctement
float zbuf(vec2 uv)
{
  float n = 0.1; // camera z near
  float f = 100.0; // camera z far
  float z = texture2D(nMap, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main()
{
  int i = 0;
  vec4 sum = vec4(0.0);

  const float step = 1.0/500.; // distance sur laquelle faire le blur
  float sumi = 1.;

  float z = zbuf(gl_TexCoord[0].st);
  //  float dof = 0.6; // entre 0 et 1
  float blurMin = 0.4;
  float intensity = (z <= dof) ? blurMin : ((z - dof) * (1. - dof) / (1. - blurMin));

  float cos1 = 0.999847;
  float sin1 = 0.0174524;
  for( i=0; i<25; i++ )
    {
      vec2 offs;
      float x = intensity * float(i%5 - 2) * step;
      float y = intensity * float(i/5 - 2) * step;

      offs.x = x * cos1 - y * sin1;
      offs.y = x * sin1 + y * cos1;

      vec4 tmp = texture2D(cMap, gl_TexCoord[0].st + offs);
      float coef = kernel[i];
      sumi += coef;
      sum += tmp * coef;
    }
  sum /= sumi;


// Barres pour le style
//  if ((gl_TexCoord[0].s > 0.05 && gl_TexCoord[0].s < 0.06)
//      || (gl_TexCoord[0].t > 0.05 && gl_TexCoord[0].t < 0.06))
//    sum /= 4.;

// Pseudo brouillard
//  sum = (sum + 2.*vec4(zbuf(gl_TexCoord[0].st))) / 3.;

// Le bruit rigolo pour Zavie
//      sum += sin(float(time % 250) / 250. * 314.) / 50.;

//Affichage avant/après
//   if( gl_TexCoord[0].s>0.505 )
//     {
//       sum = texture2D(cMap, gl_TexCoord[0].xy);
//     }
//   if( gl_TexCoord[0].s<=0.501 && gl_TexCoord[0].s>=0.5)
//     {
//       sum = vec4(1.0, 0.0, 0.0, 1.0);
//     }

//  sum = vec4(1.0, 0.0, 0.0, 1.0);

  // transition par texture
  sum *= smoothstep(trans-0.03, trans, texture2D(sMap, gl_TexCoord[0].st).x);

  // fondu vers noir
  gl_FragColor = mix(vec4(fadeLuminance), sum, fade);
}
