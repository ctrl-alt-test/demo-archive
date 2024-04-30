//
// font.hh for demotest
// Made by nicuveo <crucuny@gmail.com>
// Rewrote by Zavie ;)
//

#ifndef		FONT_HH
# define	FONT_HH

#include "fontwrapper.hh"
#include "textureid.hh"

namespace Font
{
  class Font
  {
  public:
    static Font* create(const HDC & hdc,
			const char* name,
			int height,
			int weight,
			Texture::id textureId = Texture::font);

    void displayGlyph(unsigned int i) const;

    float width(const char* str) const;
    float width(const char* s, float height) const;
//     float height(const char* s, float width)  const;

    void print(const char* text, float height, float x, float y) const;
//     void print_w(const char* text, float width,  float x, float y) const;

  private:
    Font(Wrapper & wrapper, Texture::id textureId);

    int _glyphPositionX(int i) const;
    int _glyphPositionY(int i) const;
    void _createTexture();
//     void create_lists();
    void _print(const char* text, float ratio, float x, float y) const;

    Wrapper		_wrapper;
    unsigned int	_maxWidth;
    unsigned int	_maxHeight;
    int			_maxBaseLine;
    Texture::id		_textureId;
  };
}

#endif		// FONT_HH
