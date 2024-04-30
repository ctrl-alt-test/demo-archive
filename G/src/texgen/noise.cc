//=============================================================================
//
// Perlin Noise
//
//=============================================================================

// Improved Perlin noise.
// Original Perlin noise implementation can be found at :
// http://mrl.nyu.edu/~perlin/doc/oscar.html#noise

#include "../interpolation.hxx"
#include "../randomness.hh"
#include "../sys/msys.h"

#define		MOD	0xff

static bool		noise_ready = false;
static int		permut[256];
static const char	gradient[32][4] =
  {
    { 1,  1,  1,  0}, { 1,  1,  0,  1}, { 1,  0,  1,  1}, { 0,  1,  1,  1},
    { 1,  1, -1,  0}, { 1,  1,  0, -1}, { 1,  0,  1, -1}, { 0,  1,  1, -1},
    { 1, -1,  1,  0}, { 1, -1,  0,  1}, { 1,  0, -1,  1}, { 0,  1, -1,  1},
    { 1, -1, -1,  0}, { 1, -1,  0, -1}, { 1,  0, -1, -1}, { 0,  1, -1, -1},
    {-1,  1,  1,  0}, {-1,  1,  0,  1}, {-1,  0,  1,  1}, { 0, -1,  1,  1},
    {-1,  1, -1,  0}, {-1,  1,  0, -1}, {-1,  0,  1, -1}, { 0, -1,  1, -1},
    {-1, -1,  1,  0}, {-1, -1,  0,  1}, {-1,  0, -1,  1}, { 0, -1, -1,  1},
    {-1, -1, -1,  0}, {-1, -1,  0, -1}, {-1,  0, -1, -1}, { 0, -1, -1, -1},
  };


static void	InitNoise()
{
  Rand rand;
  noise_ready = true;
  for (unsigned int i = 0; i < 256; ++i)
  {
    permut[i] = rand.igen() & MOD;
  }
}

//
// Function finding out the gradient corresponding to the coordinates
//
static int	Indice(int i, int j, int k, int l)
{
  return permut[(l + permut[(k + permut[(j + permut[i & MOD])
					& MOD])
			    & MOD])
		& MOD] & 0x1f;
}

//
// Functions computing the dot product of the vector and the gradient
//
inline
static float	Prod(float a, char b)
{
//   if (b > 0)
//     return a;
//   if (b < 0)
//     return -a;
//   return 0;
  return ((b > 0) - (b < 0)) * a;
}

static float	Dot_prod(float x1, char x2,
			 float y1, char y2,
			 float z1, char z2,
			 float t1, char t2)
{
  return Prod(x1, x2) + Prod(y1, y2) + Prod(z1, z2) + Prod(t1, t2);
}

//
// Noise function, returning the Perlin Noise at a given point
//
float			Noise(float x, float y, float z, float t)
{
  if (!noise_ready)
    InitNoise();

  // The unit hypercube containing the point
  const int x1 = msys_ifloorf(x);
  const int y1 = msys_ifloorf(y);
  const int z1 = msys_ifloorf(z);
  const int t1 = msys_ifloorf(t);

  const int x2 = x1 + 1;
  const int y2 = y1 + 1;
  const int z2 = z1 + 1;
  const int t2 = t1 + 1;

  // The 16 corresponding gradients
  const char * g0000 = gradient[Indice(x1, y1, z1, t1)];
  const char * g0001 = gradient[Indice(x1, y1, z1, t2)];
  const char * g0010 = gradient[Indice(x1, y1, z2, t1)];
  const char * g0011 = gradient[Indice(x1, y1, z2, t2)];
  const char * g0100 = gradient[Indice(x1, y2, z1, t1)];
  const char * g0101 = gradient[Indice(x1, y2, z1, t2)];
  const char * g0110 = gradient[Indice(x1, y2, z2, t1)];
  const char * g0111 = gradient[Indice(x1, y2, z2, t2)];
  const char * g1000 = gradient[Indice(x2, y1, z1, t1)];
  const char * g1001 = gradient[Indice(x2, y1, z1, t2)];
  const char * g1010 = gradient[Indice(x2, y1, z2, t1)];
  const char * g1011 = gradient[Indice(x2, y1, z2, t2)];
  const char * g1100 = gradient[Indice(x2, y2, z1, t1)];
  const char * g1101 = gradient[Indice(x2, y2, z1, t2)];
  const char * g1110 = gradient[Indice(x2, y2, z2, t1)];
  const char * g1111 = gradient[Indice(x2, y2, z2, t2)];

  // The 16 vectors
  float dx1 = x - x1;
  float dx2 = x - x2;
  float dy1 = y - y1;
  float dy2 = y - y2;
  float dz1 = z - z1;
  float dz2 = z - z2;
  float dt1 = t - t1;
  float dt2 = t - t2;

  // The 16 dot products
  const float b0000 = Dot_prod(dx1, g0000[0], dy1, g0000[1],
			       dz1, g0000[2], dt1, g0000[3]);
  const float b0001 = Dot_prod(dx1, g0001[0], dy1, g0001[1],
			       dz1, g0001[2], dt2, g0001[3]);
  const float b0010 = Dot_prod(dx1, g0010[0], dy1, g0010[1],
			       dz2, g0010[2], dt1, g0010[3]);
  const float b0011 = Dot_prod(dx1, g0011[0], dy1, g0011[1],
			       dz2, g0011[2], dt2, g0011[3]);
  const float b0100 = Dot_prod(dx1, g0100[0], dy2, g0100[1],
			       dz1, g0100[2], dt1, g0100[3]);
  const float b0101 = Dot_prod(dx1, g0101[0], dy2, g0101[1],
			       dz1, g0101[2], dt2, g0101[3]);
  const float b0110 = Dot_prod(dx1, g0110[0], dy2, g0110[1],
			       dz2, g0110[2], dt1, g0110[3]);
  const float b0111 = Dot_prod(dx1, g0111[0], dy2, g0111[1],
			       dz2, g0111[2], dt2, g0111[3]);
  const float b1000 = Dot_prod(dx2, g1000[0], dy1, g1000[1],
			       dz1, g1000[2], dt1, g1000[3]);
  const float b1001 = Dot_prod(dx2, g1001[0], dy1, g1001[1],
			       dz1, g1001[2], dt2, g1001[3]);
  const float b1010 = Dot_prod(dx2, g1010[0], dy1, g1010[1],
			       dz2, g1010[2], dt1, g1010[3]);
  const float b1011 = Dot_prod(dx2, g1011[0], dy1, g1011[1],
			       dz2, g1011[2], dt2, g1011[3]);
  const float b1100 = Dot_prod(dx2, g1100[0], dy2, g1100[1],
			       dz1, g1100[2], dt1, g1100[3]);
  const float b1101 = Dot_prod(dx2, g1101[0], dy2, g1101[1],
			       dz1, g1101[2], dt2, g1101[3]);
  const float b1110 = Dot_prod(dx2, g1110[0], dy2, g1110[1],
			       dz2, g1110[2], dt1, g1110[3]);
  const float b1111 = Dot_prod(dx2, g1111[0], dy2, g1111[1],
			       dz2, g1111[2], dt2, g1111[3]);

  // Then the interpolations, down to the result
  dx1 = smoothStep5(dx1);
  dy1 = smoothStep5(dy1);
  dz1 = smoothStep5(dz1);
  dt1 = smoothStep5(dt1);

  const float b111 = mix(b1110, b1111, dt1);
  const float b110 = mix(b1100, b1101, dt1);
  const float b101 = mix(b1010, b1011, dt1);
  const float b100 = mix(b1000, b1001, dt1);
  const float b011 = mix(b0110, b0111, dt1);
  const float b010 = mix(b0100, b0101, dt1);
  const float b001 = mix(b0010, b0011, dt1);
  const float b000 = mix(b0000, b0001, dt1);

  const float b11 = mix(b110, b111, dz1);
  const float b10 = mix(b100, b101, dz1);
  const float b01 = mix(b010, b011, dz1);
  const float b00 = mix(b000, b001, dz1);

  const float b1 = mix(b10, b11, dy1);
  const float b0 = mix(b00, b01, dy1);

  return mix(b0, b1, dx1);
}

// ============================================================================
