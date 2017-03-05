//
// Text sticker
//

#include "fontsticker.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>

namespace Font
{
  Sticker::Sticker(const HDC & hdc,
		   const char * name, int height, int weight):
    _wrapper(hdc, name, height, weight, FALSE)
  {
  }

  Sticker::~Sticker()
  {
    _wrapper.destroy();
  }

  void Sticker::selectFont(const char * name, int height, int weight, int italic)
  {
    _wrapper.destroy();
    _wrapper.init(name, height, weight, italic);
  }

  float Sticker::width(const char* str) const
  {
    assert(str);

    const unsigned char* s = (unsigned char*) str;
    float res = 0;
    float tmp = 0;

    if (*s)
      res -= _wrapper.widths(*s - 32).abcfA;

    while (*s)
    {
      res += (_wrapper.widths(*s - 32).abcfA +
	      _wrapper.widths(*s - 32).abcfB +
	      _wrapper.widths(*s - 32).abcfC);
      tmp = _wrapper.widths(*s - 32).abcfC;
      ++s;
    }

    res -= tmp;

    return res;
  }

  void Sticker::print(const char* text, float x, float y, unsigned char r, unsigned char g, unsigned char b) const
  {
    _wrapper.rasterize(text, x, y, r, g, b);
  }
}
