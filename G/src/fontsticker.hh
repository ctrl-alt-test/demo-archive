//
// Text sticker
//

#ifndef		FONT_STICKER_HH
# define	FONT_STICKER_HH

#include "fontwrapper.hh"
#include "textureid.hh"

namespace Font
{
  class Sticker
  {
  public:
    Sticker(const HDC & hdc,
	    const char * name, int height, int weight);
    ~Sticker();

    void selectFont(const char * name, int height, int weight = FW_NORMAL, int italic = FALSE);

    float width(const char* str) const;
    float width(const char* s, float height) const;
    void print(const char* text, float x, float y, unsigned char r, unsigned char g, unsigned char b) const;

  private:
    Wrapper	_wrapper;
  };
}

#endif		// FONT_STICKER_HH
