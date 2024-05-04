#ifndef WIN32_FONT_HH
#define WIN32_FONT_HH

#include <windows.h>

namespace Gfx
{
	struct DrawArea;
}

namespace platform
{
	enum FontWeight
	{
		thin	= FW_THIN,
		light	= FW_ULTRALIGHT,
		normal	= FW_REGULAR,
		bold	= FW_BOLD,
		black	= FW_BLACK,
	};

	class Win32Font
	{
	public:
		Win32Font(const HDC& hdc,
				  const char* fontName,
				  int height,
				  FontWeight weight);
		~Win32Font();

		long GetTextWidth(const char* str) const;
		void Print(const Gfx::DrawArea& drawArea, float x, float y, const char* str) const;

	private:
		HDC				m_hDC;
		HFONT			m_hFont;
		unsigned int	m_displayLists;
	};
}

#endif // WIN32_FONT_HH
