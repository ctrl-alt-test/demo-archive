//
// font.cc for demotest
// Made by nicuveo <crucuny@gmail.com>
//



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

#include "font.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>

#if DEBUG
#include <cstdio>
#endif // !DEBUG

#include "textures.hh"



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Implementation

Font::Font(const char* name, unsigned height, Texture::id textureId)
  : name_(name), height_(height), offset_(0), size_x_(0), size_y_(0), _textureId(textureId)
{
  list_ = glGenLists(224);
}

Font::~Font()
{
  glDeleteLists(list_, 224);
}


Font*
Font::create(const HDC & hdc, int xres, int yres,
	     const char* name, unsigned height, unsigned weight,
	     Texture::id textureId)
{
  Font* res;
  HFONT font;

  res = new Font(name, height, textureId);
  font = CreateFont(-1 * height,
		    0,
		    0,
		    0,
		    weight,
		    FALSE,
		    FALSE,
		    FALSE,
		    ANSI_CHARSET,
		    OUT_TT_PRECIS,
		    CLIP_DEFAULT_PRECIS,
		    PROOF_QUALITY,
		    FF_DONTCARE|DEFAULT_PITCH,
		    name);
  SelectObject(hdc, font);
  wglUseFontBitmaps(hdc, 32, 224, res->list_);

  res->create_limits(hdc);
  res->create_textures(xres, yres);
  res->create_lists();

  return res;
}


void
Font::create_limits(const HDC & hdc)
{
  MAT2 m2 = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
  GLYPHMETRICS m;
  float max = 0;
  unsigned i;

  GetCharABCWidthsFloat(hdc, 32, 255, width_);

  for (unsigned i = 32; i < 256; ++i)
  {
    GetGlyphOutline(hdc, i, GGO_METRICS, &m, 0, 0, &m2);
    int h = m.gmBlackBoxY;
    int y = m.gmptGlyphOrigin.y;

    if (h - y > offset_)
      offset_ = h - y;
    if (y > size_y_)
      size_y_ = y;

    if (width_[i - 32].abcfB > max)
      max = width_[i - 32].abcfB;
  }

  size_x_ = msys_ifloorf(max + 1);
  size_y_ += offset_;

  for (i = 0; i < 12 && 16 * size_x_ > 1 << i; ++i)
    ;
  assert(i >= 4);
  tex_x_ = 1 << (i - 4);

  for (i = 0; i < 12 && 16 * size_y_ > 1 << i; ++i)
    ;
  assert(i >= 4);
  tex_y_ = 1 << (i - 4);
}

void
Font::create_textures(int xres, int yres)
{
  // Texture generation
  glEnable(GL_TEXTURE_2D);

  const unsigned int width = 16 * tex_x_;
  const unsigned int height = 16 * tex_y_;

  assert(tex_x_ > 0 && tex_y_ > 0);

  Texture::Unit & target = Texture::list[_textureId];
  target.setRenderTexture(width, height, GL_ALPHA, GL_ALPHA);

  // Pushing
  glPushAttrib(GL_LIST_BIT | GL_COLOR_BUFFER_BIT);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, width, height);
  glClearColor(1.f, 1.f, 1.f, 0);
  glColor4ub(255, 255, 255, 255);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glListBase(0);
  for (unsigned i = 0; i < 224; ++i)
  {
    // Character rendering
    glDisable(GL_TEXTURE_2D);

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2f(xres / 2.f - width_[i].abcfA, yres / 2.f);
    glCallList(list_ + i);

    //
    // FIXME : callage en X, qui foire selon le rapport de résolution
    //
    glEnable(GL_TEXTURE_2D);
    target.renderToTexture((i % 16) * tex_x_, (i / 16) * tex_y_,
			   xres  / 2, yres / 2 - offset_,
			   msys_ifloorf(width_[i].abcfB + 1), size_y_);
  }
  glPopAttrib();
}

void
Font::create_lists()
{
  DBG("Creation des listes pour le texte");

  glPushAttrib(GL_LIST_BIT);
  glListBase(0);

  for (unsigned i = 0; i < 224; ++i)
  {
    const float x1 = ((i % 16) * tex_x_)                   / (16.f * tex_x_);
    const float x2 = ((i % 16) * tex_x_ + width_[i].abcfB) / (16.f * tex_x_);
    const float y1 = ((i / 16) * tex_y_)                   / (16.f * tex_y_);
    const float y2 = ((i / 16) * tex_y_ + size_y_)         / (16.f * tex_y_);

    // Call list initialisation
    glNewList(list_ + i, GL_COMPILE);
    OGL_ERROR_CHECK("New list");
    glTranslated(width_[i].abcfA, 0., 0.);

    glBegin(GL_QUADS);
    glTexCoord2f(x1, y2); glVertex2f(0,               0);
    glTexCoord2f(x2, y2); glVertex2f(width_[i].abcfB, 0);
    glTexCoord2f(x2, y1); glVertex2f(width_[i].abcfB, (GLfloat) size_y_);
    glTexCoord2f(x1, y1); glVertex2f(0,               (GLfloat) size_y_);
    glEnd();

    glTranslated(width_[i].abcfC + width_[i].abcfB, 0., 0.);

    assert(width_[i].abcfB > 0);
    assert(width_[i].abcfB > 0.1f);

    glEndList();
  }

  glPopAttrib();
}


float
Font::width(unsigned char c, unsigned height)
{
  float ratio = height / ((float) size_y_);

  return width_[c - 32].abcfB * ratio;
}

float
Font::height(unsigned char c, unsigned width)
{
  float ratio = width / width_[c - 32].abcfB;

  return size_y_ * ratio;
}

float
Font::width(const char* str, unsigned height)
{
  const unsigned char* s = (unsigned char*) str;
  float ratio = height / ((float) size_y_);
  float res = 0;
  float tmp = 0;

  assert(s);

  if (*s)
    res -= width_[*s - 32].abcfA;

  while (*s)
  {
    res += width_[*s - 32].abcfA + width_[*s - 32].abcfB + width_[*s - 32].abcfC;
    tmp  = width_[*s - 32].abcfC;
    s += 1;
  }

  res -= tmp;

  return res * ratio;;
}

float
Font::height(const char* str, unsigned w)
{
  float r = width(str, size_y_);

  assert(r);

  return (w * size_y_) / r;
}


void
Font::print(const char* str, unsigned height, float x, float y)
{
  if (!str || !*str)
    return;

  const unsigned char* text = (unsigned char*) str;
  const float ratio = height / ((float) size_y_);

  _print(str, ratio, x, y);
}

void
Font::print_w(const char* str, unsigned width, float x, float y)
{
  if (!str || !*str)
    return;

  const unsigned char* text = (unsigned char*) str;
  float ratio = height(str, width) / size_y_;

  _print(str, ratio, x, y);
}

void
Font::_print(const char* text, float ratio, float x, float y)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity();
  glTranslatef(x, y, 0);
  glScalef(ratio, ratio, 0);
  glTranslatef(-width_[*text - 32].abcfA, 0, 0);

  Texture::list[_textureId].use(0);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glPushAttrib(GL_LIST_BIT);
  glListBase(list_ - 32);
  glCallLists(msys_strlen(text), GL_UNSIGNED_BYTE, text);
  glPopAttrib();

  glPopMatrix();
}

/*
#if DEBUG

void
Font::dump(const unsigned char* tmp) const
{
  FILE *fd = fopen("tex_dump.raw", "w");
  if (0 == fd)
    return;

//   if (size_x_ * size_y_ * 224 != fwrite(tmp, 1, size_x_ * size_y_ * 224, fd))
//     DBG("Failed to dump texture! T_T");
  if (4 * 512 * 512 != fwrite(tmp, 1, 4 * 512 * 512, fd))
    DBG("Failed to dump texture! T_T");

  fclose (fd);
}

#endif // !DEBUG
*/
