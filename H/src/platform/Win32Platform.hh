#ifndef WIN32_PLATFORM_HH
#define WIN32_PLATFORM_HH

#include "IPlatform.hh"
#include <windows.h>

namespace platform
{
	/// <summary>
	/// Wrapper abstracting the boiler plate necessary for window and
	/// draw context creation.
	/// </summary>
	class Win32Platform // : public IPlatform
	{
	public:
		/// <param name="windowTitle">Text visible in the title bar of the window.</param>
		/// <param name="width">Horizontal resolution.</param>
		/// <param name="height">Vertical resolution.</param>
		Win32Platform(const char* windowTitle,
					  int windowWidth, int windowHeight,
					  int monitorLeft, int monitorTop,
					  int monitorWidth, int monitorHeight,
					  bool fullscreen);
		~Win32Platform();

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
		HINSTANCE	m_hInstance;
		HWND		m_hWnd;

		// Set as public as a temporary hack, until we figure a better
		// abstraction.
	public:
		HDC			m_hDC;

	private:
		HGLRC		m_hRC;
	};
}

#endif // WIN32_PLATFORM_HH
