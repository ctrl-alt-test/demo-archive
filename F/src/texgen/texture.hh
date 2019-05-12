// ============================================================================
//
// Texture channel, with its operators
//
// ============================================================================

#ifndef		TEXTURE_CHANNEL_HH
# define	TEXTURE_CHANNEL_HH

#define TWO_PI	(2 * 3.14159265358979f)

namespace Texture
{
  class Channel
  {
  public:
    // Constructors and destructor
    Channel();
    Channel(unsigned int width, unsigned int height);
    Channel(unsigned int width, unsigned int height, unsigned char * data, int stride = 1);
    Channel(const Channel &);
    ~Channel();

    // Data index
    unsigned int UnsecureGetIndex(unsigned int x, unsigned int y) const;
    unsigned int GetIndex(int x, int y) const;
    unsigned int GetClampedIndex(int x, int y) const;

    // Accessors
    unsigned int	Width() const;
    unsigned int	Height() const;
    unsigned int	Size() const;
    const float *	operator () () const;
    const float		operator [] (unsigned int i) const;
    float &		operator [] (unsigned int i);
    float &		Pixel(int i, int j);
    float		Pixel(int i, int j) const;
    float		Bilinear(float i, float j) const;

    // Generators
    void	ApplyFunction(float (*f)(unsigned int width,
					 unsigned int height,
					 unsigned int i,
					 unsigned int j,
					 float x,
					 unsigned char hparam,
					 unsigned char vparam),
			      unsigned char hparam,
			      unsigned char vparam);
//  void	operator () (const unsigned int, const float);

    void	Clear();
    void	Flat(float v);
    void	Gradient(float a, float b, float c, float d);
    void	AddCuts(float density, float length);
    void	Cells(unsigned int cells);
    void	Cells2(unsigned int cells);
    void	Conic();
    void	Radial();
    void	Square();
    void	Diamond();
    void	Fade(float angle);
    void	Perlin(unsigned char frequency,
		       unsigned char stretch,
		       unsigned char,
		       float time);
    void	Sinus(unsigned char hfrequency,
		      unsigned char vfrequency);
    void	Cosinus(unsigned char hfrequency,
			unsigned char vfrequency);
    void	Random();

    // Filters
    void	Cut(int n = 1);
    void	Abs();
    void	Pow(float exp);
    void	Clamp(float min, float max);
    void	Scale(float min, float max);
    void	ClampAndScale(float min, float max);
    void	Mod();
    void	Sin();
    void	NegativeNormalized();
    void	Segment(float);

    void	ApplyFilter(const float * filter,
			    unsigned int size,
			    float factor);
    // Painting
    void	FloodFill(unsigned x, unsigned y, float newColor);
    void	Line(float x0, float y0, float x1, float y1);
    void	Linei(int x0, int y0, int x1, int y1);

    void	Bump();
    void	Blur();
    void	Sharpen();
    void	Emboss();
    void	EdgeDetect();
    void	GaussianBlur();
    void	Dilate(float radius);
    void	HorizontalMotionBlur(float length);
    void	VerticalMotionBlur(float length);
    void	VerticalDribble();

    // Matrix
    void	Translate(float hshift, float vshift);
    void	Translatei(int hshift, int vshift);
    void	ClampedTranslatei(int hshift, int vshift);
    void	HorizontalFlip();
    void	VerticalFlip();
    void	Transpose();
    void	Rotate90();
    void	HorizontalWave(float frequency, float amplitude);
    void	VerticalWave(float frequency, float amplitude);
    void        Waves(float angle, float amplitude, const Channel& waves);
    void	AverageScale(unsigned int n, float coef);
    void        ZoomOut(float zx, float zy);

    // Operators
    void	Mix(float factor, const Channel & a);
    void	Mask(const Channel & mask, const Channel & a);
    void	MinimizeTo(const Channel & a);
    void	MaximizeTo(const Channel & a);

    void	subCopy(const Channel & a,
			unsigned int srcx, unsigned int srcy,
			unsigned int dstx, unsigned int dsty,
			unsigned int width, unsigned int height);

    void	operator += (const Channel & a);
    void	operator -= (const Channel & a);
    void	operator *= (const Channel & a);

    void	operator *= (float a);
    void	operator /= (float a);
    void	Mult (float a) { *this *= a; }

//   Channel	Min(const Channel &) const;
//   Channel	Max(const Channel &) const;
//   Channel	operator + (const Channel &) const;
//   Channel	operator - (const Channel &) const;
//   Channel	operator * (const Channel &) const;
    Channel&	operator = (const Channel &);

  private:
    unsigned int	_size;
    unsigned int	_width;
    unsigned int	_height;
    float *		_data;

    void		CellsGen(unsigned int cells, unsigned int n);
  };

  inline unsigned int	Channel::Width() const
  {
    return _width;
  }

  inline unsigned int	Channel::Height() const
  {
    return _height;
  }

  inline unsigned int	Channel::Size() const
  {
    return _size;
  }

  inline const float *	Channel::operator () () const
  {
    return _data;
  }

  inline float &	Channel::operator [] (unsigned int i)
  {
    // assert(i < _size);
    return _data[i];
  }

}

#endif			// TEXTURE_CHANNEL_HH
