#ifndef XLIB_PLATFORM_HH
#define XLIB_PLATFORM_HH

#include "IPlatform.hh"
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windef.h>
#endif // _WIN32
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

namespace platform
{
	class XLibPlatform // : public IPlatform
	{
	public:
		/// <param name="windowTitle">Text visible in the title bar of the window.</param>
		/// <param name="width">Horizontal resolution.</param>
		/// <param name="height">Vertical resolution.</param>
		XLibPlatform(const char* windowTitle,
					 int width, int height,
					 int monitorLeft, int monitorTop,
					 int monitorWidth, int monitorHeight,
					 bool fullscreen);
		~XLibPlatform();

		int			GetWidth() const;
		int			GetHeight() const;

		/// <summary>
		/// Get the system time, in milliseconds.
		/// </summary>
		long		GetTime() const;

		/// <summary>
		/// Sleep for a given duration, in milliseconds.
		/// </summary>
		void		Sleep(long milliseconds) const;
		void		SetWindowTitle(const char* title) const;
		void		AddInputHandler(CharacterHandler characterHandler, KeyHandler keyHandler, MouseHandler mouseHandler);
		bool		HandleMessages() const;
		void		SwapBuffers() const;

	private:
		Display*	m_dpy;
		GLXContext	m_glc;
		Window		m_win;
		int			m_width;
		int			m_height;

#ifdef ENABLE_KEYBOARD_AND_MOUSE
		KeyHandler		m_handleKey;
		MouseHandler	m_handleMouse;
#endif // ENABLE_KEYBOARD_AND_MOUSE
	};
}

// This hack prevents "None", which is defined as a macro in glx.h,
// from polluting the name space. :-(
#ifndef XLIB_PLATFORM_INCLUDE
#ifdef None
#undef None
#endif
#endif

#endif // XLIB_PLATFORM_HH
