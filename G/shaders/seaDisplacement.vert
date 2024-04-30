#version 120

uniform int time;
uniform sampler2D tex5; // FFT

//[
float getShift();
void exportColor();
//]

#define PI 3.1415926535

vec3 GerstnerWave(float t, vec2 Xo, float A, vec2 uk)
{
  //
  // Gerstner
  //
  // Xo = (xo, yo) : position initiale
  // A : amplitude
  // K : vecteur direction de la vague
  // k = 2pi/lambda : amplitude de K
  //
  // x = Xo - sum(Ki/ki * Ai * sin(dot(Ki, Xo) - wi * t + phii))
  // y =      sum(        Ai * cos(dot(Ki, Xo) - wi * t + phii))
  //
  // Relation entre frequence et amplitude :
  // w²(K) = g * k  (g = 9.801 m/s²)
  //
  // Relation quand la profondeur D est faible :
  // w²(K) = g * k * tanh(k * D)
  //
  float lambda = A / 20.;
  vec2 k = uk * 2. * PI / lambda;
  float w = sqrt(9.801 * 2. * PI / lambda) * 0.2;

  float x = -uk.x * A * sin(dot(k, Xo) - w * t);
  float z = -uk.y * A * sin(dot(k, Xo) - w * t);
  float y =         A * cos(dot(k, Xo) - w * t);

  return vec3(x, y, z);
}

vec3 getDisplacement(vec2 uv)
{
  float t = 0.001 * float(time);
  vec2 Xo = vec2(uv.x, uv.y + getShift());

  // A est la hauteur des vagues, en mètres (attention, ça s'additionne).
  // La longueur d'onde se règle avec lambda, dans la fonction GerstnerWave
  float A[8] = float[8]( 3., 2., 1.5, 0.97, 0.71, 0.53, 0.31, 0.23 );
  vec2 uk[8] = vec2[8](vec2(-0.961,  0.276), // 286
		       vec2(-0.5,    0.866), // 330
		       vec2( 0.53,   0.848), // 32
		       vec2(-0.326, -0.945), // 199

		       vec2( 0.695, -0.719), // 136
		       vec2( 0.087, -0.996), // 175
		       vec2( 0.891,  0.454), // 63
		       vec2(-0.992,  0.122));// 277

  // 16000: début de la chute
  // 17000: fin de la chute
  // 24000: fin du premier passage
  float fade[8] = float[8]( smoothstep(19500., 24000., float(time)),
			    smoothstep(19000., 23000., float(time)),
			    smoothstep(18500., 22000., float(time)),
			    smoothstep(18000., 21000., float(time)),
			    smoothstep(17500., 20000., float(time)),
			    smoothstep(17000., 19000., float(time)),
			    smoothstep(16500., 18000., float(time)),
			    smoothstep(16000., 17000., float(time)) );

  vec3 wave = vec3(0);
  for (int i = 0; i < 8; ++i)
    wave += fade[i] * GerstnerWave(t, Xo, A[i], uk[i]);

  vec2 uvFFT = vec2(abs(0.5-uv.x)*2.2, fract(uv.y + getShift()));
  float fftLevel = texture2D(tex5, uvFFT).x;
  fftLevel *= smoothstep(0.07, 0.15, fftLevel);
  float fftHeight = 100.* fftLevel * smoothstep(17000., 16000., float(time));

  return vec3(0., fftHeight, 0.) + wave;
}

void exportCustomColor(float h, vec3 n)
{
  exportColor();
}
