// -*- glsl -*-

uniform sampler2D cMap;
uniform sampler2D c2Map;

// godrays, ou light scattering
vec4 scatter()
{
  const int NUM_SAMPLES = 100 ;

  float exposure = 0.00075;
  float density = 0.98;
  float weight = 5.;

  vec2 lightPositionOnScreen = vec2(0.5, 0.45);

  vec2 pos = gl_TexCoord[0].st;
  vec2 deltaTextCoord = vec2(gl_TexCoord[0].st - lightPositionOnScreen.xy);
  deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;
  float illuminationDecay = 1.0;

  float col = 0.;
  for(int i=0; i < NUM_SAMPLES ; i++)
    {
      pos -= deltaTextCoord;
      // on regarde le zbuffer pour savoir où arrêter les rayons
      float sample = smoothstep(0.999, 1., texture2D(c2Map, pos).r);;
      sample *= illuminationDecay * weight;
      col += sample;
    }
  col *= exposure;
  return vec4(col);
}

// glow un peu allégé
vec4 glow()
{
  vec4 sum = vec4(0.);
  vec2 texcoord = gl_TexCoord[0].st;

  for(int i = -3; i < 3; i++)
    {
      for (int j = -3; j < 3; j++)
        {
          float x = float(i);
          float y = float(j);
          vec2 v = vec2(x, y);

          float coef = length(v);
          coef = 1. / max(1., coef);
          vec2 pos = texcoord + vec2(x, y) * 0.003;
          pos = clamp(pos, 0.001, 0.999);
          vec4 tex = texture2D(cMap, pos);
          coef *= 1. - tex.a;
          sum += tex * coef;
        }
    }
  return sum * 0.25;
}

void main()
{
  vec4 col = texture2D(cMap, gl_TexCoord[0].st);
  col += glow();
  col += scatter();
  gl_FragColor = col;
}
