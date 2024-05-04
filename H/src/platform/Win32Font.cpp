#include "Win32Font.hh"
#include "gfx/OpenGL/OpenGLLayer.hh"
#include <cassert>
#include <gl/gl.h>

using namespace platform;

#define NB_GLYPHS 224

Win32Font::Win32Font(const HDC& hdc,
					 const char* fontName,
					 int height,
					 FontWeight weight):
	m_hDC(hdc)
{
	m_hFont = CreateFont(-1 * height,
		0,
		0,
		0,
		weight,
		FALSE, // Italic
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		fontName);
	assert(m_hFont != 0);

	SelectObject(m_hDC, m_hFont);
	m_displayLists = glGenLists(NB_GLYPHS);
	assert(m_displayLists != 0);

	wglUseFontBitmaps(m_hDC, 32, NB_GLYPHS, m_displayLists);
}

Win32Font::~Win32Font()
{
	glDeleteLists(m_displayLists, NB_GLYPHS);
}

long Win32Font::GetTextWidth(const char* str) const
{
	SelectObject(m_hDC, m_hFont);

	SIZE textSize;
	GetTextExtentPoint32(m_hDC, str, strlen(str), &textSize);
	return textSize.cx;
}

void Win32Font::Print(const Gfx::DrawArea& drawArea, float x, float y, const char* str) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, drawArea.viewport.width, 0.0, drawArea.viewport.height, 0.0, 1.0);
	glRasterPos2f(x, y);

	glListBase(0);
	for (const unsigned char* c = (unsigned char*)str; *c != '\0'; ++c)
	{
		glCallList(m_displayLists + *c - 32);
	}
}
