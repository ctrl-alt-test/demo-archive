//
// Win API font wrapper
//

#include "fontwrapper.hh"

#include "sys/msys_debug.h"
#include "sys/msys_glext.h"
#include "sys/msys_malloc.h"

using namespace Font;

Wrapper::Wrapper(const HDC & hdc,
		 const char* name, int height, int weight, int italic):
  _hdc(hdc)
{
  init(name, height, weight, italic);
}

void Wrapper::init(const char* name, int height, int weight, int italic)
{
  _lists = glGenLists(NB_GLYPHS);
  _hfont = CreateFont(-1 * height,
		      0,
		      0,
		      0,
		      weight,
		      italic,
		      FALSE,
		      FALSE,
		      ANSI_CHARSET,
		      OUT_TT_PRECIS,
		      CLIP_DEFAULT_PRECIS,
		      ANTIALIASED_QUALITY,
		      FF_DONTCARE|DEFAULT_PITCH,
		      name);
  assert(_hfont != NULL);
  SelectObject(_hdc, _hfont);
  wglUseFontBitmaps(_hdc, 32, NB_GLYPHS, _lists);

  _widths = (ABCFLOAT *)msys_mallocAlloc(NB_GLYPHS * sizeof(ABCFLOAT));
  _metrics = (GLYPHMETRICS *)msys_mallocAlloc(NB_GLYPHS * sizeof(GLYPHMETRICS));

  GetCharABCWidthsFloat(_hdc, 32, 32 + NB_GLYPHS - 1, _widths);

  MAT2 m2 = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
  for (unsigned int i = 0; i < NB_GLYPHS; ++i)
  {
    GetGlyphOutline(_hdc, i + 32, GGO_METRICS, _metrics + i, 0, 0, &m2);
  }
}

void Wrapper::destroy()
{
  glDeleteLists(_lists, NB_GLYPHS);
  msys_mallocFree(_metrics);
  msys_mallocFree(_widths);
  DeleteObject(_hfont);
}

/*
GLYPHMETRICS Wrapper::metrics(int i) const
{
  GLYPHMETRICS metrics;

  MAT2 m2 = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
  GetGlyphOutline(_hdc, i + 32, GGO_METRICS, &metrics, 0, 0, &m2);

  return metrics;
}
*/

void Wrapper::rasterize(const char * str, float x, float y, unsigned char r, unsigned char g, unsigned char b) const
{
  assert(str && *str);
  assert(*str);

  SelectObject(_hdc, _hfont);

  glPushAttrib(GL_LIST_BIT | GL_COLOR_BUFFER_BIT);
//   glListBase(_lists - 32);
  glListBase(0);

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

#if SMOOTHER_FONT
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glColor4ub(r, g, b, 128);
  const float gitter[] = { 0.125,  0.375,
			  -0.125, -0.375,
			  -0.375,  0.125,
			   0.375, -0.125 };
  for (unsigned int pass = 0; pass < 4; ++pass)
  {
    glRasterPos2f(x + gitter[2 * pass], y + gitter[2 * pass + 1]);
    for (const unsigned char * c = (unsigned char*)str; *c != 0; ++c)
    {
      glCallList(_lists + *c - 32);
    }
  }
#else
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4ub(r, g, b, 255);
  glRasterPos2f(x, y);
  for (const unsigned char * c = (unsigned char*)str; *c != NULL; ++c)
  {
    glCallList(_lists + *c - 32);
  }
#endif

  glPopAttrib();
}
