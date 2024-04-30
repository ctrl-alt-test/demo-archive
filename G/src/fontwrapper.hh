//
// Win API font wrapper
//

#ifndef		FONT_WRAPPER_HH
# define	FONT_WRAPPER_HH

#include "sys/msys.h" // FIXME: juste pour quelques #define

#define NB_GLYPHS	224 // Nombre de glyphes chargés
#define SMOOTHER_FONT	1

namespace Font
{
  class Wrapper
  {
  public:
    Wrapper(const HDC & hdc, const char* name,
	    int height, int weight, int italic);

    void init(const char* name, int height, int weight, int italic);
    void destroy();

    inline const ABCFLOAT &	widths(int i) const	{ return _widths[i]; }
    inline const GLYPHMETRICS &	metrics(int i) const	{ return _metrics[i]; }
//     GLYPHMETRICS metrics(int i) const;

    void rasterize(const char * str, float x, float y, unsigned char r, unsigned char g, unsigned char b) const;

  private:
    const HDC &		_hdc;
    HFONT		_hfont;

    unsigned int	_lists;
    ABCFLOAT *		_widths;
    GLYPHMETRICS *	_metrics;
  };
}

#endif		// FONT_WRAPPER_HH
