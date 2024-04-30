
vec3 GerstnerWave(float t, vec2 X, float A, float L, float steepness, vec2 D)
{
  //
  // Gerstner function
  // http://http.developer.nvidia.com/GPUGems/gpugems_ch01.html
  //
  // X: initial 2D position
  // D: wave 2D direction
  // A: wave amplitude
  // t: time
  //
  // w: wave sharpness
  //   = sqrt(2pi.g / L)
  //   g: 9.801 m/s²
  // L: wavelength

  // Q: steepness, 0 .. 1/(w * A)
  // c: speed, c = sqrt(L * g / 2PI)

#define PI 3.1415926535

  float w = 2. * PI / L;
  float c = sqrt(9.801 * 2. * PI / L);
  float Q = steepness/(w * A);

  float theta = w * dot(D, X) + c * t;
  float x = Q * A * D.x * cos(theta);
  float y = Q * A * D.y * cos(theta);
  float z =     A * sin(theta);

  return vec3(x, z, y);
}


float waveHeight(float x)
{
  float quiet = cos(x) * 0.5 + 0.5;
  float choppy = 1. - abs(sin(x * 0.5));
  return mix(choppy, quiet, pow(choppy, 3.));
}

vec2 waves(vec2 uv, float time, sampler2D randomTexture)
{
  vec2 ripple = vec2(0.);

  for (int i = 0; i < 16; ++i)
  {
    float f = pow(1.2, float(i+1));

    vec2 hash = hash2DTex(randomTexture, float(i));
    float direction = hash.x * 2. * PI;
    vec2 v = vec2(sin(direction), cos(direction));

    // Break pattern by offsetting waves.
    mat2 rot = mat2(v, vec2(v.y, -v.x));
    float offset = texture2D(randomTexture, (f/256.)*vec2(3.,10.)*(rot * uv)).b * 3. / f;
    float d = f * (dot(uv, v) - time + offset);

    // Make waves appear only on patches.
    float amplitude = pow(0.9, float(i)) * smoothstep(0.4, 1., texture2D(randomTexture, 4.*(f/256.) * (rot * (uv - v * time))).b);

    float h = 1e-2*f;
    float d1 = d - h;
    float d2 = d + h;
    float p1 = amplitude * waveHeight(d1);
    float p2 = amplitude * waveHeight(d2);
    vec2 n = normalize(vec2(p2 - p1, 2. * h));
    ripple += v * n.x;
  }
  return ripple;
}
