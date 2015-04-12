//
// font.hh for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#ifndef FONT_HH_
# define FONT_HH_



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

# include "windows.h"
// # include "sys/msys_debug.h"
// # include "texturemanager.hh"

#include "textureid.hh"


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Declarations

class Font
{
  public:
    ~Font();

    static Font* create(const HDC & hdc,
			int xres, int yres,
			const char* name,
			unsigned height = 66,
			unsigned weight = FW_NORMAL,
			Texture::id textureId = Texture::font);

    const char* name() const;

    float width (unsigned char c, float height) const;
    float height(unsigned char c, float width)  const;
    float width (const char* s,   float height) const;
    float height(const char* s,   float width)  const;

    void print  (const char* text, float height, float x, float y) const;
    void print_w(const char* text, float width,  float x, float y) const;

private:
  Font(const char* name, unsigned height, Texture::id textureId);
  void _print(const char* text, float ratio, float x, float y) const;

#if DEBUG
  void dump(const unsigned char* tmp) const;
#endif // !DEBUG

  void create_limits(const HDC & hdc);
  void create_textures(int xres, int yres);
  void create_lists();

  const char* name_;
  int height_;

  int tex_x_;
  int tex_y_;
  int offset_;
  int size_x_;
  int size_y_;

  unsigned int   list_;
  unsigned int   texid_; // FIXME : utiliser plutôt une Texture::Unit
  Texture::id	_textureId;
  ABCFLOAT width_[224];
};



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

# include "font.hxx"



#endif /* !FONT_HH_ */
