// ============================================================================
//
// Texture class
//
// ============================================================================

#include "array.hxx"
#include "interpolation.hxx"
#include "algebra/vector3.hh"
#include "noise.hh"
#include "texture.hh"

#include "../sys/msys.h"

#define DEFAULT_SIDE	512
#define MAX_SIDE	2048
#define MAX_SIZE	(MAX_SIDE * MAX_SIDE)
// #define			TEXTURE_SIZE	TEXTURE_SIDE * TEXTURE_SIDE

namespace Texture
{
  // ============================================== Constructors and destructor

  Channel::Channel():
    _width(DEFAULT_SIDE),
    _height(DEFAULT_SIDE),
    _size(DEFAULT_SIDE * DEFAULT_SIDE),
    _rand()
  {
    _data = new float[_size];
    Clear();
  }

  Channel::Channel(unsigned int width, unsigned int height,
		   unsigned char * data, int stride):
    _width(width),
    _height(height),
    _size(width * height),
    _rand()
  {
    const float byteToFloat = 1.f / 255.f;
    _data = new float[_size];

    // Y axis upside down for OpenGL way of handling texture coordinates
    for (unsigned int j = 0, k = height - 1; j < height; ++j, k--)
      for (unsigned int i = 0; i < width; ++i)
      {
	_data[width * k + i] = data[width * j + i] * byteToFloat;
      }
  }

  Channel::Channel(unsigned int width, unsigned int height):
    _width(width),
    _height(height),
    _size(width * height),
    _rand()
  {
    _data = new float[_size];
  }

  Channel::Channel(const Channel & ref):
    _width(ref.Width()),
    _height(ref.Height()),
    _size(ref.Size()),
    _rand(ref._rand)
  {
    _data = new float[_size];
    for (unsigned int i = 0; i < _size; ++i)
      _data[i] = ref[i];
  }

  Channel::~Channel()
  {
    delete[] _data;
  }

  // =============================================================== Data index

  // Modulo toujours positif.
  // TODO: mettre ailleurs ?

  // Session 1 - 6,22
  /*
  static int imod(int x, int n)
  {
    return (x + n) % n;
  }
  */

  // Session 2 - 6,4
  /*
  inline static int imod(int x, int n)
  {
    return (x + n) % n;
  }
  */

  // Session 3 - 2,29
  static int imod(int x, int n)
  {
    return (x + n) & (n - 1);
  }

  // Session 4 - 2,43
  /*
  inline static int imod(int x, int n)
  {
    return (x + n) & (n - 1);
  }
  */

  // Session 5 - 7,22
  /*
  static int imod(int x, int n)
  {
    x %= n;
    return x < 0 ? x + n : x;
  }
  */

  // Session 6 - 7,09
  /*
  inline static int imod(int x, int n)
  {
    x %= n;
    return x < 0 ? x + n : x;
  }
  */

  inline
  unsigned int	Channel::UnsecureGetIndex(unsigned int x, unsigned int y) const
  {
    return x + y * _width;
  }

  unsigned int	Channel::GetIndex(int x, int y) const
  {
    return UnsecureGetIndex(imod(x, _width), imod(y, _height));
  }

  unsigned int	Channel::GetClampedIndex(int x, int y) const
  {
    return UnsecureGetIndex(clamp(x, 0, _width - 1), clamp(y, 0, _height - 1));
  }

  // ================================================================ Accessors

  float	Channel::operator [] (unsigned int i) const
  {
    assert(i < _size);
    return _data[i];
  }

  float &          Channel::Pixel(int i, int j)
  {
    return _data[GetIndex(i, j)];
  }

  float            Channel::Pixel(int i, int j) const
  {
    return _data[GetIndex(i, j)];
  }

  // Fetch bilineaire
  float            Channel::Bilinear(float i, float j) const
  {
    int x = msys_ifloorf(i);
    int y = msys_ifloorf(j);

    float i_ratio = i - (float)(x);
    float j_ratio = j - (float)(y);
    float i_opposite = 1 - i_ratio;
    float j_opposite = 1 - j_ratio;
    float result = ((Pixel(  x,   y) * i_opposite +
		     Pixel(x+1,   y) * i_ratio) * j_opposite +
		    (Pixel(  x, y+1) * i_opposite +
		     Pixel(x+1, y+1) * i_ratio) * j_ratio);
    return result;
  }

  // ================================================================= Generators

  void		Channel::ApplyFunction(float (*f)(unsigned int width,
						  unsigned int height,
						  unsigned int i,
						  unsigned int j,
						  float x,
						  unsigned char hparam,
						  unsigned char vparam),
				       unsigned char hparam,
				       unsigned char vparam)
  {
    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
      {
	const unsigned int index = i + shift;
	_data[index] = f(_width, _height, i, j, _data[index], hparam, vparam);
      }
    }
  }

  // --------------------------------------------------------------------------

  void		Channel::Clear()
  {
    msys_memset(_data, 0, _size * sizeof(float));
  }

  void		Channel::Flat(float v)
  {
    for (unsigned int i = 0; i < _size; ++i)
      _data[i] = v;
  }

  void		Channel::Gradient(float a, float b, float c, float d)
  {
    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      const float hfactor = (float)j / (float)_height;
      const float top = mix(a, b, hfactor);
      const float bottom = mix(d, c, hfactor);
      for (unsigned int i = 0; i < _width; ++i)
      {
	_data[i + shift] = mix(top, bottom, (float)i / (float)_width);
      }
    }
  }

  void		Channel::AddCuts(float density, // traits par pixel
				 float length)
  {
    assert(length > 0 && length <= 1.f);
    assert(density > 0);
    const unsigned int number = 1 + (unsigned int)msys_ifloorf(_width * density);
    const unsigned int ilength = 1 + (unsigned int)msys_ifloorf(_width * length);

    for (unsigned int i = 0; i < number; ++i)
    {
      int x =  _rand.igen() % _width;
      int y =  _rand.igen() % _height;
      int dx = _rand.igen() % ilength + 1;
      int dy = _rand.igen() % ilength + 1;
      int sx = _rand.igen() % 2;
      int sy = _rand.igen() % 2;

      Linei(x, y, (sx == 0 ? x + dx : x - dx), (sy == 0 ? y + dy : y - dy));
    }
  }

  static int	CellsPrivate(unsigned int width, unsigned int height,
			     int i, int j,
			     unsigned int * cell_x,
			     unsigned int * cell_y,
			     unsigned int cells,
                             unsigned int n)
  {
    // FIXME : améliorer l'algo (de n² à n.log(n) je crois)

    int distance = width * height;
    int distance2 = width * height;

    for (unsigned int k = 0; k < cells; k++)
    {
      int dx = abs((int)cell_x[k] - i);
      int dy = abs((int)cell_y[k] - j);
      if (dx > (int)width / 2)
	dx = width - dx;
      if (dy > (int)height / 2)
	dy = height - dy;

      const int min = dx * dx + dy * dy;
      if (min < distance) {
	distance2 = distance;
	distance = min;
      }
      else if (min < distance2)
	distance2 = min;
    }
    return n == 2 ? distance2 : distance;
  }

  void		Channel::CellsGen(unsigned int cells, unsigned int n)
  {
    unsigned int * cell_x = new unsigned int[cells];
    unsigned int * cell_y = new unsigned int[cells];

    for (unsigned int i = 0; i < cells; ++i)
    {
      cell_x[i] = _rand.igen() % _width;
      cell_y[i] = _rand.igen() % _height;
    }

    float max = 0;
    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
      {
	const int distance = CellsPrivate (_width, _height, i, j, cell_x, cell_y, cells, n);
	const float pixel = msys_sqrtf((float)distance);
	(*this)[i + shift] = pixel;
	if (pixel > max)
	  max = pixel;
      }
    }
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] /= max; // normalize the result
    delete [] cell_x;
    delete [] cell_y;
  }

  void		Channel::Cells(unsigned int cells)
  {
    CellsGen(cells, 1);
  }

  void		Channel::Cells2(unsigned int cells)
  {
    CellsGen(cells, 2);
  }

  static float RadialFunction(unsigned int width, unsigned int height,
			      unsigned int i, unsigned int j, float,
			      unsigned char, unsigned char)
  {
    const int dx = width / 2 - i;
    const int dy = height / 2 - j;
    if (dx == 0 && dy == 0) return 0.f;
    const float distance = msys_sqrtf(float(dx * dx + dy * dy));
    const float y = msys_atan2f(dy / distance, dx / distance);
    return (y + TWO_PI / 2.f) / TWO_PI;
  }

  void            Channel::Radial()
  {
    ApplyFunction(RadialFunction, 0, 0);
  }

  static float ConicFunction(unsigned int width, unsigned int height,
			     unsigned int i, unsigned int j, float,
			     unsigned char, unsigned char)
  {
    const int dx = (width / 2 - i);
    const int dy = (height / 2 - j);
    const float distance = msys_sqrtf(float(dx * dx + dy * dy));
    float pixel = 1.f - 2.f * distance / float(width); // FIXME : adapter
    if (pixel > 1.f)
      pixel = 1.f;
    return pixel;
  }

  void		Channel::Conic()//unsigned char,// height, char shift)
  {
    ApplyFunction(ConicFunction, 0, 0);
  }

  static float SquareFunction(unsigned int width, unsigned int height,
			      unsigned int i, unsigned int j, float,
			      unsigned char, unsigned char)
  {
    const float dw = 2 * (2 * i <= width ? i : width - i) / (float)width;
    const float dh = 2 * (2 * j <= height ? j : height - j) / (float)height;
    return msys_min(dw, dh);

    //   float pixel = 1 - 2 * distance / width; // FIXME : adapter
    //   if (pixel > 1)
    //     pixel = 1;
    //   if (pixel < 0)
    //     pixel = 0;
    //   return pixel;
  }

  void		Channel::Square()//unsigned char,// height, char shift)
  {
    ApplyFunction(SquareFunction, 0, 0);
  }

  static float DiamondFunction(unsigned int width, unsigned int height,
			       unsigned int i, unsigned int j, float,
			       unsigned char, unsigned char)
  {
    const float dw = (2 * i <= width ? i : width - i) / (float)width;
    const float dh = (2 * j <= height ? j : height - j) / (float)height;
    return dw + dh;
  }

  void		Channel::Diamond()
  {
    ApplyFunction(DiamondFunction, 0, 0);
  }

  void		Channel::Fade(float angle)
  {
#define DEG2RAD (3.1415926535897932384626433832f / 180.f)
    const float alpha = DEG2RAD * angle;
    const float c = cosf(alpha);
    const float s = sinf(alpha);
    const float add = 0.5f * (s + c);
    const float sub = 0.5f * (s - c);
    Gradient(0.5f - add, 0.5f - sub, 0.5f + add, 0.5f + sub);
  }

  // static float PerlinFunction(unsigned int i, unsigned int j, float,
  // 			    unsigned char frequency, unsigned char stretch)
  // {
  //   const float v = frequency / (float)stretch;
  //   const float w = frequency;

  //   const float y = (frequency * (float)j) / TEXTURE_SIDE;
  //   const float x = (frequency * (float)i) / (stretch * TEXTURE_SIDE);

  //   const float noise1 = Noise(x, y, time, 0.f);
  //   const float noise2 = Noise(x - v, y, time, 0.f);
  //   const float noise3 = Noise(x, y - w, time, 0.f);
  //   const float noise4 = Noise(x - v, y - w, time, 0.f);
  //   const float noise = ((v - x) * (w - y) * noise1 +
  // 		       x       * (w - y) * noise2 +
  // 		       (v - x) * y       * noise3 +
  // 		       x       * y       * noise4) / (v * w);
  //   return noise;
  // }

  void		Channel::Perlin(unsigned char frequency,
				unsigned char stretch,
				unsigned char,// variant,
				float time)
  {
    const float v = frequency / (float)stretch;
    const float w = frequency;
    for (unsigned int j = 0; j < _height; ++j)
    {
      const float y = (frequency * (float)j) / _height;
      for (unsigned int i = 0; i < _width; ++i)
      {
	const float x = (frequency * (float)i) / (stretch * _width);

	//
	// FIXME : faire une version avec un seul appel a noise au lieu de 4
	//

	const float noise1 = Noise(x, y, time, 0.f);
	const float noise2 = Noise(x - v, y, time, 0.f);
	const float noise3 = Noise(x, y - w, time, 0.f);
	const float noise4 = Noise(x - v, y - w, time, 0.f);
	const float noise = ((v - x) * (w - y) * noise1 +
			     x       * (w - y) * noise2 +
			     (v - x) * y       * noise3 +
			     x       * y       * noise4) / (v * w);
	_data[i + j * _width] = noise;
      }
    }
  }

  static float SinusFunction(unsigned int width, unsigned int height,
			     unsigned int i, unsigned int j, float,
			     unsigned char hfrequency, unsigned char vfrequency)
  {
    return (sinf(TWO_PI / width * hfrequency * i) *
	    sinf(TWO_PI / height * vfrequency * j));
  }

  void		Channel::Sinus(unsigned char hfrequency, unsigned char vfrequency)
  {
    ApplyFunction(SinusFunction, hfrequency, vfrequency);
  }

  static float CosinusFunction(unsigned int width, unsigned int height,
			       unsigned int i, unsigned int j, float,
			       unsigned char hfrequency, unsigned char vfrequency)
  {
    return (cosf(TWO_PI / width * hfrequency * i) *
	    cosf(TWO_PI / height * vfrequency * j));
  }

  void		Channel::Cosinus(unsigned char hfrequency, unsigned char vfrequency)
  {
    ApplyFunction(CosinusFunction, hfrequency, vfrequency);
  }

  void		Channel::Random()
  {
      for (unsigned int i = 0; i < _width * _height; ++i)
      {
	_data[i] = _rand.fgen();
      }
  }

  // ================================================================== Filters

  // ------------------------------------------------------ Histogram functions


  // fait une coupure, juste avant b
  static float oneCut(float x, float a, float b)
  {
    if (x < a || x > b) return x;

    const float cut = b - 0.01f;
    if (x < cut)
      return interpolate(x, a, cut);
    else
      return interpolate(x, b, cut);
  }

  static float cut(float x, int n)
  {
    const int div = n + 1; // nombre de sections
    for (int i = 0; i < n; i++)
      if (x < (float)(i+1) / div)
	return oneCut(x, (float)i / div, (float)(i+1) / div);
    return interpolate(x, (float)n / div, 1.f);
  }

  // Fait un modulo
  void		Channel::Cut(int n)
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] = cut((*this)[i], n);
  }

  // Passe toutes les valeurs en absolu
  void		Channel::Abs()
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] = msys_fabsf((*this)[i]);
  }

  void		Channel::Pow(float exp)
  {
    for (unsigned int i = 0; i < _size; ++i)
    {
      const float v = (*this)[i];
      (*this)[i] = (v <= 0.f ? 0.f : msys_powf(v, exp));
    }
  }

  // Majore les valeurs supérieures à max, et minore les valeurs
  // inférieures à min
  void		Channel::Clamp(float min, float max)
  {
    for (unsigned int i = 0; i < _size; ++i)
    {
      if ((*this)[i] < min)
	(*this)[i] = min;
      if ((*this)[i] > max)
	(*this)[i] = max;
    }
  }

  // Répartit les valeurs entre start et end
  void		Channel::Scale(float start, float end)
  {
    float min = (*this)[0];
    float max = (*this)[0];

    for (unsigned int i = 1; i < _size; ++i)
    {
      if ((*this)[i] < min)
	min = (*this)[i];
      if ((*this)[i] > max)
	max = (*this)[i];
    }
	assert(min < max);
    for (unsigned int i = 0; i < _size; ++i)
    {
      const float pixel = interpolate((*this)[i], min, max);
      (*this)[i] = mix(start, end, pixel);
    }
  }

  // Clampe puis répartit les valeurs entre 0 et 1
  void		Channel::ClampAndScale(float min, float max)
  {
    assert(min != max);
    for (unsigned int i = 0; i < _size; ++i)
    {
      const float scaled = ((*this)[i] - min) / (max - min);
      (*this)[i] = clamp(scaled);
    }
  }

  // Effectue un modulo sur un flottant pour le remettre entre 0 et 1.
  // C'est une fonction continue : mod(1.1) == mod(0.9)
  static float mod(float x)
  {
    x = msys_fabsf(x);
    const int ipart = msys_ifloorf(x);
    const float fpart = x - (float)ipart;
    if (ipart % 2 == 1)
      return 1 - fpart;
    else
      return fpart;
  }

  void		Channel::Mod()
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] = mod((*this)[i]);
  }

  void		Channel::Sin()
  {
    for (unsigned int i = 0; i < _size; ++i)
    {
      const float rad = (*this)[i] * TWO_PI / 2.f;
      (*this)[i] = sinf(rad - TWO_PI / 4.f) * 0.5f + 0.5f;
    }
  }

  void		Channel::NegativeNormalized()
  {
    Scale(1.f, 0);
  }

  void		Channel::Segment(float limit)
  {
    for (unsigned int i = 0; i < _size; ++i)
      while ((*this)[i] > limit)
      {
	(*this)[i] = (*this)[i] - limit;
      }
  }


  // ---------------------------------------------------------- Pixel functions

  void		Channel::FloodFill(unsigned x, unsigned y, float newColor)
  {
    Array<int> stack(10000);

    float oldColor = (*this)[x + y * _width];
    stack.add(x + y * _width);

    while (stack.size > 0)
    {
	bool spanLeft = false;
	bool spanRight = false;
	int pos = stack.last();
	stack.pop();
	x = imod(pos % _width, _width);
	y = imod(pos / _width, _width);
        int y1 = y;

        while ((y != _height + y1 - 1) && (*this)[GetIndex(x, y1)] == oldColor)
	  y1--;
        y1 = imod(y1 + 1, _width);

      while ((*this)[GetIndex(x, y1)] == oldColor)
	  {
	    (*this)[GetIndex(x, y1)] = newColor;
          if (spanLeft == false && ((*this)[GetIndex(x - 1, y1)] == oldColor))
	      {
                stack.add(imod(x - 1, _width) + y1 * _width);
                spanLeft = true;
	      }
          else if(spanLeft == true && ((*this)[GetIndex(x - 1, y1)] != oldColor))
	      {
                spanLeft = false;
	      }
          if (spanRight == false && ((*this)[GetIndex(x + 1, y1)] == oldColor))
	      {
                stack.add(imod(x + 1, _width) + y1 * _width);
                spanRight = true;
	      }
          else if (spanRight == true && ((*this)[GetIndex(x + 1, y1)] != oldColor))
	      {
                spanRight = false;
	      }
          y1 = (y1 + 1) % _width;
	  }
      }
  }

// A deplacer autre part ?
  template<typename A>
  void swap(A & x, A & y)
  {
    A tmp = x;
    x = y;
    y = tmp;
  }

  // Bresenham algorithm
  void		Channel::Line(float x0, float y0, float x1, float y1)
  {
    int xi0 = msys_ifloorf(x0 * _width);
    int yi0 = msys_ifloorf(y0 * _height);
    int xi1 = msys_ifloorf(x1 * _width);
    int yi1 = msys_ifloorf(y1 * _height);
    Linei(xi0, yi0, xi1, yi1);
  }

  // Bresenham algorithm
  void		Channel::Linei(int x0, int y0, int x1, int y1)
  {
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
      swap(x0, y0);
      swap(x1, y1);
    }

    if (x0 > x1)
    {
      swap(x0, x1);
      swap(y0, y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = deltax / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
      (*this)[steep ? GetIndex(y, x) : GetIndex(x, y)] = 1.f;
      error = error - deltay;
      if (error < 0)
      {
	y += ystep;
	error += deltax;
      }
    }
  }

  void		Channel::ApplyFilter(const float * filter,
				     unsigned int size,
				     float factor)
  {
    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
      for (unsigned int i = 0; i < _width; ++i)
      {
	float value = 0;
	for (unsigned int dj = 0; dj < size; ++dj)
	  for (unsigned int di = 0; di < size; ++di)
	  {
	    value += filter[di + size * dj] * save[GetIndex(i + di, j + dj)];
	  }
	_data[GetIndex(i + size / 2, j + size / 2)] = factor * value;
      }
  }

  // --------------------------------------------------------------------------
  void		Channel::Bump()
  {
    static const float bumpFilter[9] = {  0, -1, 0,
					 -1,  2, 0,
					  0,  0, 0 };
    ApplyFilter(bumpFilter, 3, 1);
  }

  void		Channel::Emboss()
  {
    static const float embossFilter[9] = { -2, -1, 0,
					   -1,  1, 1,
					    0,  1, 2 };
    ApplyFilter(embossFilter, 3, 1);
  }

  void		Channel::EdgeDetect()
  {
    static const float edgeFilter[9] = { 0,  1, 0,
					 1, -4, 1,
					 0,  1, 0 };
    ApplyFilter(edgeFilter, 3, 1);
  }

  void		Channel::Sharpen()
  {
    static const float sharpenFilter[9] = {  0, -1, 0,
					    -1,  5, -1,
					     0, -1, 0 };
    ApplyFilter(sharpenFilter, 3, 1);
  }

  void		Channel::Blur()
  {
    static const float blurFilter[9] = { 0, 1, 0,
					 1, 4, 1,
					 0, 1, 0 };
    ApplyFilter(blurFilter, 3, 0.125f);
  }

  void		Channel::GaussianBlur()
  {
    static const float gaussianFilter[25] = { 1,  4,  7,  4, 1,
					      4, 16, 26, 16, 4,
					      7, 26, 41, 26, 7,
					      4, 16, 26, 16, 4,
					      1,  4,  7,  4, 1 };
    ApplyFilter(gaussianFilter, 5, 0.003663003663f);
  }

  void		Channel::HorizontalMotionBlur(float length)
  {
    assert(length > 0 && length <= 1.f);
    const int ilength = 1 + (unsigned int)(_width * length);

    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
      {
        float total = 0.f;
        for (int shift = -ilength; shift < ilength; ++shift)
          total += save[GetIndex(shift, j)];

        for (unsigned int i = 0; i < _width; ++i)
          {
            (*this)[i + j * _width] = total / (2 * ilength);
            total -= save[GetIndex(i - ilength, j)];
            total += save[GetIndex(i + ilength, j)];
          }
      }
  }

  void		Channel::Dilate(float radius)
  {
    assert(radius > 0 && radius <= 0.5f);
    const int iradius = 1 + (unsigned int)(_width * radius);
    const float invMaxDistance = 1.f / msys_sqrtf(float(iradius * iradius));

    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
      for (unsigned int i = 0; i < _width; ++i)
      {
	float maxValue = 0;

	for (int kj = 0; kj < iradius; ++kj)
	  for (int ki = 0; ki < iradius; ++ki)
	  {
	    const float factor = 1.f - msys_min(1.f, msys_sqrtf(float(ki * ki + kj * kj)) * invMaxDistance);
	    maxValue = msys_max(maxValue, factor * save[GetIndex(i + ki, j + kj)]);
	    maxValue = msys_max(maxValue, factor * save[GetIndex(i - ki, j + kj)]);
	    maxValue = msys_max(maxValue, factor * save[GetIndex(i + ki, j - kj)]);
	    maxValue = msys_max(maxValue, factor * save[GetIndex(i - ki, j - kj)]);
	  }

	(*this)[i + j * _width] = maxValue;
      }
  }

  void		Channel::VerticalMotionBlur(float length)
  {
    assert(length > 0 && length <= 1.f);
    const int ilength = 1 + (unsigned int)(_height * length);

    const Channel save(*this);

    for (unsigned int i = 0; i < _width; ++i)
      {
        float total = 0.f;
        for (int shift = -ilength; shift < ilength; ++shift)
          total += save[GetIndex(i, shift)];

        for (unsigned int j = 0; j < _height; ++j)
          {
            (*this)[i + j * _width] = total / (2 * ilength);
            total -= save[GetIndex(i, j - ilength)];
            total += save[GetIndex(i, j + ilength)];
          }
      }
  }

  void		Channel::VerticalDribble()
  {
    for (unsigned int j = 0; j < _height; ++j)
      for (unsigned int i = 0; i < _width; ++i)
      {
	const unsigned int index = GetIndex(i, j);
	const float pixel = _data[index];
	const float previous = _data[GetIndex(i, j - 1)];
	_data[index] = (pixel + 9 * pixel * previous) / 10;
      }
  }


  // ---------------------------------------------------------- Shape functions

  void		Channel::Translate(float hshift, float vshift)
  {
    assert(hshift > -1.f && hshift < 1.f);
    assert(vshift > -1.f && vshift < 1.f);
    const int ihshift = (unsigned int)msys_ifloorf(_width * hshift);
    const int ivshift = (unsigned int)msys_ifloorf(_height * vshift);
    Translatei(ihshift, ivshift);
  }

  void		Channel::Translatei(int hshift, int vshift)
  {
    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
	(*this)[i + shift] = save[GetIndex(i + hshift, j + vshift)];
    }
  }

  void		Channel::ClampedTranslatei(int hshift, int vshift)
  {
    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
	(*this)[i + shift] = save[GetClampedIndex(i + hshift, j + vshift)];
    }
  }

  void		Channel::HorizontalFlip()
  {
    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width / 2; ++i)
      {
	const unsigned int indice = i + shift;
	const unsigned int flip_indice = (_width - i - 1) + shift;

	const float temp = (*this)[indice];
	(*this)[indice] = (*this)[flip_indice];
	(*this)[flip_indice] = temp;
      }
    }
  }

  void		Channel::VerticalFlip()
  {
    for (unsigned int j = 0; j < _height / 2; ++j)
    {
      const unsigned int shift = j * _height;
      const unsigned int flip_shift = (_height - j - 1) * _width;
      for (unsigned int i = 0; i < _width ; ++i)
      {
	const unsigned int indice = i + shift;
	const unsigned int flip_indice = i + flip_shift;

	const float temp = (*this)[indice];
	(*this)[indice] = (*this)[flip_indice];
	(*this)[flip_indice] = temp;
      }
    }
  }

  void		Channel::Transpose()
  {
    // FIXME : refaire sans allouer de nouvelle texture

    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
	(*this)[i + shift] = save[j + _height * i];
    }
  }

  void		Channel::Rotate90()
  {
    // FIXME : refaire sans allouer de nouvelle texture

    const Channel save(*this);

    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
	(*this)[i + shift] = save[j + _width * (_height - i - 1)];
    }
  }

  void		Channel::HorizontalWave(float frequency, float amplitude)
  {
    const Channel save(*this);
    amplitude *= _width;

    for (unsigned int j = 0; j < _height; ++j)
      {
        const unsigned int shift = j * _width;
        const float s = sinf(frequency * j * TWO_PI / _height);
        const float wshift = amplitude * s;
        for (unsigned int i = 0; i < _width; ++i)
          (*this)[i + shift] = save.Bilinear(i + wshift, (float)j);
      }
  }

  void		Channel::VerticalWave(float frequency, float amplitude)
  {
    const Channel save(*this);
    amplitude *= _height;

    for (unsigned int i = 0; i < _width; ++i)
      {
        const float s = sinf(frequency * i * TWO_PI / _height);
        const float wshift = amplitude * s;
        for (unsigned int j = 0; j < _height; ++j)
          (*this)[i + j * _width] = save.Bilinear((float)i, j + wshift);
      }
  }

  void		Channel::Waves(float angle, float amplitude, const Channel& waves)
  {
    const Channel save(*this);
    amplitude *= _height;
    float _cos = msys_cosf(angle);
    float _sin = msys_sinf(angle);

    for (unsigned int j = 0; j < _height; ++j)
      for (unsigned int i = 0; i < _width; ++i)
        {
          float d = amplitude * waves[i + j * _width];
          (*this)[i + j * _width] = save.Bilinear(i + d * _cos, j + d * _sin);
        }
  }

  void		Channel::AverageScale(unsigned int n, float coef)
  {
    assert(coef != 0);
    const Channel save(*this);
    int mult = 1;
    float c = 1.f;
    for (unsigned int k = 0; k < n; k++)
    {
      mult *= 2;
      c *= coef;
      const float invC = 1.f / c;
      for (unsigned int j = 0; j < _height; ++j)
      {
	const unsigned int shift = j * _width;
	for (unsigned int i = 0; i < _width; ++i)
	  (*this)[i + shift] += save[GetIndex(i*mult, j*mult)] * invC;
      }
    }
  }

  void		Channel::ZoomOut(float zx, float zy)
  {
    const Channel save(*this);
    for (unsigned int j = 0; j < _height; ++j)
    {
      const unsigned int shift = j * _width;
      for (unsigned int i = 0; i < _width; ++i)
        (*this)[i + shift] = save.Bilinear(i * zx, j * zy);
    }
  }

  // ================================================================ Operators

  void		Channel::Mix(float factor, const Channel & a)
  {
    const float invf = 1.f - factor;
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] = (*this)[i] * invf + a[i] * factor;
  }

  void		Channel::Mask(const Channel & mask, const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
    {
      const float invf = 1.f - mask[i];
      (*this)[i] = (*this)[i] * invf + a[i] * mask[i];
    }
}

  void		Channel::MinimizeTo(const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      if (a[i] > (*this)[i])
	(*this)[i] = a[i];
  }

  void		Channel::MaximizeTo(const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      if (a[i] < (*this)[i])
	(*this)[i] = a[i];
  }

  void		Channel::subCopy(const Channel & a,
				 unsigned int srcx, unsigned int srcy,
				 unsigned int dstx, unsigned int dsty,
				 unsigned int width, unsigned int height)
  {
    assert(srcx + width <= a.Width());
    assert(srcy + height <= a.Height());
    assert(dstx + width <= _width);
    assert(dsty + height <= _height);

    for (unsigned int j = 0; j < height; ++j)
    {
      const unsigned int srcShift = (srcy + j) * a.Width();
      const unsigned int dstShift = (dsty + j) * _width;

      for (unsigned int i = 0; i < width; ++i)
	(*this)[dstx + i + dstShift] = a[srcx + i + srcShift];
    }
  }

  void		Channel::operator += (const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] += a[i];
  }

  void		Channel::operator -= (const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] -= a[i];
  }

  void		Channel::operator *= (const Channel & a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] *= a[i];
  }

  Channel&	Channel::operator = (const Channel & a)
  {
    if (_width != a.Width() || _height != a.Height())
    {
      delete[] _data;
      _width = a.Width();
      _height = a.Height();
      _size = a.Size();
      _data = new float[_size];
    }
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] = a[i];

    return *this;
  }


  void		Channel::operator *= (float a)
  {
    for (unsigned int i = 0; i < _size; ++i)
      (*this)[i] *= a;
  }

  void		Channel::operator /= (float a)
  {
    operator *= (1 / a);
  }


  // Export, for standalone texgen

#ifdef TEXGEN_ONLY
  // FIXME: static variable - might lead to race conditions
  static unsigned char data[4 * MAX_SIZE];

  static unsigned char floatToByte(float value)
  {
    return (unsigned char)(clamp(value) * 255);
  }

  //
  // Y axis up side down for OpenGL way of handling texture coordinates
  //
  const unsigned char * AlphaBuffer(const Channel & a)
  {
    const unsigned int size = a.Width() * a.Height();
    for (unsigned int i = 0; i < size; ++i)
      {
        data[i] = floatToByte(a[i]);
      }
    return data;
  }

  const unsigned char * RGBBuffer(const Channel & r,
                  const Channel & g,
                  const Channel & b)
  {
    const unsigned int size = r.Width() * r.Height();
    for (unsigned int i = 0; i < size; ++i)
      {
        data[3 * i] = floatToByte(r[i]);
        data[3 * i + 1] = floatToByte(g[i]);
        data[3 * i + 2] = floatToByte(b[i]);
      }
    return data;
  }

  const unsigned char * RGBABuffer(const Channel & r,
                   const Channel & g,
                   const Channel & b,
                   const Channel & a)
  {
    const unsigned int size = r.Width() * r.Height();
    for (unsigned int i = 0; i < size; ++i)
      {
        data[4 * i] = floatToByte(r[i]);
        data[4 * i + 1] = floatToByte(g[i]);
        data[4 * i + 2] = floatToByte(b[i]);
        data[4 * i + 3] = floatToByte(a[i]);
      }
    return data;
  }
#endif

}
