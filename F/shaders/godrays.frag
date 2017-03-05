uniform sampler2D nMap;

vec4 godrays(vec2 uv)
{
  const int NUM_SAMPLES = 30 ;

  float exposure = 0.0007;
  float density = 1.;
  float weight = 10.; //6.5;
  vec2 pos = uv;

  // FIXME: position du Soleil
  vec2 lightPositionOnScreen = vec2(0.8, 0.38);

  // afficher la position du soleil
  //if (length(pos - lightPositionOnScreen) < 0.05)
  //return vec4(0.);

  vec2 deltaTextCoord = vec2(uv - lightPositionOnScreen.xy);
  float dist = length(deltaTextCoord);
  deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;

  float col = 0.;
  for (int i = 0; i < NUM_SAMPLES ; i++)
    {
      pos -= deltaTextCoord;

      float sample = smoothstep(0.999, 1., texture2D(nMap, pos).r);;

      sample *= weight;
      col += sample;
    }
  col *= exposure;
  col /= pow(max(dist, 0.01), 0.2);
  // De jour:
  return vec4(col) * vec4(.6, .6, .6, 0.);
  // De nuit
  // return vec4(col) * vec4(1., 0.2, 0., 0.);
}
