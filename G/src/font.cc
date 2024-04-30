//
// font.cc for demotest
// Made by nicuveo <crucuny@gmail.com>
// Rewrote by Zavie ;)
//

#include "font.hh"

#include "camera.hh"
#include "intro.hh"
#include "textures.hh"

#include "sys/msys_debug.h"
#include "sys/msys_glext.h"

#define NB_GLYPHS_X	14  // Répartition sur la texture
#define NB_GLYPHS_Y	16
#define PADDING		4   // Bord autour des cellules

namespace Font
{
  Font::Font(Wrapper & wrapper, Texture::id textureId):
    _wrapper(wrapper), _textureId(textureId),
    _maxWidth(0), _maxHeight(0), _maxBaseLine(0)
  {
    _createTexture();
//   create_lists();
  }

  /*
  Font::~Font()
  {
    glDeleteLists(list_, NB_GLYPHS);
  }
  */

  Font * Font::create(const HDC & hdc,
		      const char* name, int height, int weight,
		      Texture::id textureId)
  {
    assert(name != NULL);
    assert(height > 0);
    DBG("Font creation: %s...", name);

    Wrapper wrapper(hdc, name, height, weight, FALSE);
    return new Font(wrapper, textureId);
  }

  int Font::_glyphPositionX(int i) const
  {
    return PADDING + (PADDING + _maxWidth) * (i % NB_GLYPHS_X);
  }

  int Font::_glyphPositionY(int i) const
  {
    return PADDING + (PADDING + _maxHeight) * (i / NB_GLYPHS_X);
  }

  void Font::_createTexture()
  {
    // Taille du rectangle majorant pour tous les glyphes
    long maxBaseX = 0;
    long maxBaseY = 0;
    for (unsigned int i = 0; i < NB_GLYPHS; ++i)
    {
      const GLYPHMETRICS & metrics = _wrapper.metrics(i);
      assert(metrics.gmBlackBoxX > 0 && metrics.gmBlackBoxY > 0);

      _maxWidth = msys_max(_maxWidth, metrics.gmBlackBoxX);
      _maxHeight = msys_max(_maxHeight, metrics.gmBlackBoxY);
      _maxBaseLine = msys_max(_maxBaseLine, int(long(metrics.gmBlackBoxY) - metrics.gmptGlyphOrigin.y));
      maxBaseX = msys_max(maxBaseX, metrics.gmptGlyphOrigin.x);
      maxBaseY = msys_max(maxBaseY, metrics.gmptGlyphOrigin.y);
    }

    // Taille de texture minimale nécessaire pour stocker les glyphes
    const unsigned int texWidth = PADDING + (PADDING + _maxWidth) * NB_GLYPHS_X;
    const unsigned int texHeight = PADDING + (PADDING + _maxHeight) * NB_GLYPHS_Y;

    // Texture generation
    glEnable(GL_TEXTURE_2D);

    Texture::Unit & target = Texture::list[_textureId];
    target.setRenderTexture(texWidth, texHeight, false, true,
			    GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE,
			    GL_CLAMP, 1.f
			    DBGARG("font"));

    glViewport(0, 0, texWidth, texHeight);
    Camera::orthoProj(texWidth, texHeight);

    glClearColor(1.f, 1.f, 1.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Clear de la texture
    for (unsigned int j = 0; j < target.height(); j += 512)
      for (unsigned int i = 0; i < target.height(); i += 512)
      {
	target.renderToTexture(i, j, 0, 0,
			       msys_min((unsigned int)512, target.width() - i),
			       msys_min((unsigned int)512, target.height() - j));
      }

    for (unsigned int i = 0; i < NB_GLYPHS; ++i)
    {
      // Rendu d'un glyphe
      glClear(GL_COLOR_BUFFER_BIT);

      const unsigned char str[] = { unsigned char(i + 32), 0 };

      const GLYPHMETRICS & metrics = _wrapper.metrics(*str - 32);
      const ABCFLOAT & widths = _wrapper.widths(*str - 32);

      assert(metrics.gmBlackBoxX > 0 && metrics.gmBlackBoxY > 0);

      _wrapper.rasterize((char *)str,
			 float(maxBaseX - widths.abcfA),
			 float(maxBaseY + metrics.gmBlackBoxY - metrics.gmptGlyphOrigin.y),
			 255, 255, 255);

      // Récupération dans la texture
      target.renderToTexture(_glyphPositionX(i) - 1,
			     _glyphPositionY(i) - 1,
			     maxBaseX - 1, maxBaseY - 1,
			     metrics.gmBlackBoxX + 2,
			     metrics.gmBlackBoxY + 2);
    }
  }

  void Font::displayGlyph(unsigned int i) const
  {
    const GLYPHMETRICS & metrics = _wrapper.metrics(i);

    const int x1 = -(PADDING / 2);
    const int x2 =  PADDING / 2 + metrics.gmBlackBoxX;
    const int y1 = -(PADDING / 2);
    const int y2 =  PADDING / 2 + metrics.gmBlackBoxY;

    const int y0 = metrics.gmptGlyphOrigin.y - metrics.gmBlackBoxY;

    const Texture::Unit & target = Texture::list[_textureId];
    const float texWidth = float(target.width());
    const float texHeight = float(target.height());

    const float u1 = float(_glyphPositionX(i) + x1) / texWidth;
    const float v1 = float(_glyphPositionY(i) + y1) / texWidth;
    const float u2 = u1 + float(x2) / texWidth;
    const float v2 = v1 + float(y2) / texHeight;

    const float vFactor = intro.aspectRatio / float(intro.xres) * float(intro.yres);

    glTranslatef(_wrapper.widths(i).abcfA, float(y0), 0);

    glBegin(GL_QUADS);
    glTexCoord2f(u1, v1); glVertex2i(x1, y1);
    glTexCoord2f(u2, v1); glVertex2i(x2, y1);
    glTexCoord2f(u2, v2); glVertex2i(x2, int(y2 * vFactor));
    glTexCoord2f(u1, v2); glVertex2i(x1, int(y2 * vFactor));
    glEnd();

    glTranslatef(_wrapper.widths(i).abcfC + _wrapper.widths(i).abcfB, -float(y0), 0);
  }

  /*
  void
  Font::create_lists()
  {
    DBG("Creation des listes pour le texte");

    list_ = glGenLists(NB_GLYPHS);
    glPushAttrib(GL_LIST_BIT);
    glListBase(0);

    for (unsigned i = 0; i < NB_GLYPHS; ++i)
    {
      glNewList(list_ + i, GL_COMPILE);
      OGL_ERROR_CHECK("New list");

      displayGlyph(i);
      glEndList();
    }

    glPopAttrib();
  }
  */

  float Font::width(const char* str) const
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

  float Font::width(const char* str, float height) const
  {
    const float ratio = height / _maxHeight;
    return width(str) * ratio;
  }

  void Font::print(const char* str, float height, float x, float y) const
  {
    const float ratio = height / _maxHeight;
    _print(str, ratio, x, y);
  }

  void Font::_print(const char* text, float ratio, float x, float y) const
  {
    if (!text || !*text)
      return;

    glLoadIdentity();
    glTranslatef(x, y, 0);
    glScalef(ratio, ratio, 0);

    glEnable(GL_TEXTURE_2D);
    Texture::list[_textureId].use();

    const unsigned char * c = (unsigned char*)text;
    glTranslatef(-_wrapper.widths(*c - 32).abcfA, float(_maxBaseLine), 0);

    while (*c != '\0')
    {
      displayGlyph(*c - 32);
      ++c;
    }

    /*
      glPushAttrib(GL_LIST_BIT);
      glListBase(list_ - 32);
      glCallLists(msys_strlen(text), GL_UNSIGNED_BYTE, text);
      glPopAttrib();
    */
  }
}
