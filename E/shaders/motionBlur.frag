uniform float screenCX;
uniform float screenCY;

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

  float inc = 0.1;
  float weight = 0.;
  for(float i = 0.; i <= 1.; i += inc)
  {
    c += texture2D(tex, clamp(uv + i * offset, vec2(0.), vec2(1., screenCY / screenCX))).rgb;
    weight += 1.;
  }
  c /= weight;
  return vec4(c, texture2D(tex, uv).a);
}
