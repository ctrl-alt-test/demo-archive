#include "Win32Platform.hh"
#include "gfx/OpenGL/wglext.h"
#include "engine/core/Debug.hh"
#include <cassert>

using namespace platform;

static const char* windowClassName = "CATWindow";

#ifdef ENABLE_KEYBOARD_AND_MOUSE

#define MAX_INPUT_HANDLERS	8

CharacterHandler s_characterHandlers[MAX_INPUT_HANDLERS] = { 0 };
KeyHandler s_keyHandlers[MAX_INPUT_HANDLERS] = { 0 };
MouseHandler s_mouseHandlers[MAX_INPUT_HANDLERS] = { 0 };
int s_nextHandler = 0;

// Virtual keys documentation:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
//
// Article on how to properly handle keys on Windows:
// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
static KeyCode::Enum GetKeyCode(WPARAM wParam)
{
	if (wParam >= '0' && wParam <= '9')
	{
		return (KeyCode::Enum)(KeyCode::key0 + (int)(wParam - (WPARAM)'0'));
	}

	if (wParam >= 'A' && wParam <= 'Z')
	{
		return (KeyCode::Enum)(KeyCode::keyA + (int)(wParam - (WPARAM)'A'));
	}

	if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
	{
		return (KeyCode::Enum)(KeyCode::keyNumpad0 + (int)(wParam - VK_NUMPAD0));
	}

	if (wParam >= VK_F1 && wParam <= VK_F24)
	{
		return (KeyCode::Enum)(KeyCode::keyF1 + (int)(wParam - VK_F1));
	}

	// MSDN virtual-key codes documentation:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731.aspx
	switch (wParam)
	{
	case VK_LBUTTON:			break;
	case VK_RBUTTON:			break;
	case VK_CANCEL:				break;
	case VK_MBUTTON:			break;
	case VK_XBUTTON1:			break;
	case VK_XBUTTON2:			break;
	case VK_BACK:		return KeyCode::keyBackspace;
	case VK_TAB:		return KeyCode::keyTab;
	case VK_CLEAR:				break;
	case VK_RETURN:		return KeyCode::keyEnter;
	case VK_SHIFT:		return KeyCode::keyLeftShift;
	case VK_CONTROL:	return KeyCode::keyLeftControl;
	case VK_MENU:		return KeyCode::keyRightAlt; // Right Alt / AltGr
	case VK_PAUSE:		return KeyCode::keyPause;
	case VK_CAPITAL:	return KeyCode::keyCapsLock;
	case VK_KANA:				break; // IME, unsupported.
	case VK_JUNJA:				break; // IME, unsupported.
	case VK_FINAL:				break; // IME, unsupported.
	case VK_HANJA:				break; // IME, unsupported.
	case VK_ESCAPE:		return KeyCode::keyEscape;
	case VK_CONVERT:			break; // IME, unsupported.
	case VK_NONCONVERT:			break; // IME, unsupported.
	case VK_ACCEPT:				break; // IME, unsupported.
	case VK_MODECHANGE:			break; // IME, unsupported.
	case VK_SPACE:		return KeyCode::keySpace;
	case VK_PRIOR:		return KeyCode::keyPageUp;
	case VK_NEXT:		return KeyCode::keyPageDown;
	case VK_END:		return KeyCode::keyEnd;
	case VK_HOME:		return KeyCode::keyHome;
	case VK_LEFT:		return KeyCode::keyArrowLeft;
	case VK_UP:			return KeyCode::keyArrowUp;
	case VK_RIGHT:		return KeyCode::keyArrowRight;
	case VK_DOWN:		return KeyCode::keyArrowDown;
	case VK_SELECT:				break;
	case VK_PRINT:				break;
	case VK_EXECUTE:			break;
	case VK_SNAPSHOT:	return KeyCode::keyPrintScreen;
	case VK_INSERT:		return KeyCode::keyInsert;
	case VK_DELETE:		return KeyCode::keyDelete;
	case VK_HELP:				break;
		//
		// All the alphanumeric keys are after VK_HELP.
		// They are handled in the loop above.
		//
	case VK_LWIN:		return KeyCode::keyLeftCommand;
	case VK_RWIN:		return KeyCode::keyRightCommand;
	case VK_APPS:		return KeyCode::keyMenu;
	case VK_SLEEP:				break;
		//
		// All the numpad numeric keys are after VK_SLEEP.
		// They are handled in the loop above.
		//
	case VK_MULTIPLY:	return KeyCode::keyNumpadMultiply;
	case VK_ADD:		return KeyCode::keyNumpadPlus;
	case VK_SEPARATOR:			break;
	case VK_SUBTRACT:	return KeyCode::keyNumpadMinus;
	case VK_DECIMAL:	return KeyCode::keyNumpadDecimal;
	case VK_DIVIDE:		return KeyCode::keyNumpadDivide;
		//
		// All the Fxx keys are after VK_DIVIDE.
		// They are handled in the loop above.
		//
	case VK_NUMLOCK:			break;
	case VK_SCROLL:		return KeyCode::keyScrollLock;
	case VK_LSHIFT:		return KeyCode::keyLeftShift;
	case VK_RSHIFT:		return KeyCode::keyRightShift;
	case VK_LCONTROL:	return KeyCode::keyLeftControl;
	case VK_RCONTROL:	return KeyCode::keyRightControl;
	case VK_LMENU:		return KeyCode::keyLeftAlt;
	case VK_RMENU:		return KeyCode::keyRightAlt;
	case VK_BROWSER_BACK:		break;
	case VK_BROWSER_FORWARD:	break;
	case VK_BROWSER_REFRESH:	break;
	case VK_BROWSER_STOP:		break;
	case VK_BROWSER_SEARCH:		break;
	case VK_BROWSER_FAVORITES:	break;
	case VK_BROWSER_HOME:		break;
	case VK_VOLUME_MUTE:		break;
	case VK_VOLUME_DOWN:		break;
	case VK_VOLUME_UP:			break;
	case VK_MEDIA_NEXT_TRACK:	break;
	case VK_MEDIA_PREV_TRACK:	break;
	case VK_MEDIA_STOP:			break;
	case VK_MEDIA_PLAY_PAUSE:	break;
	case VK_LAUNCH_MAIL:		break;
	case VK_LAUNCH_MEDIA_SELECT:break;
	case VK_LAUNCH_APP1:		break;
	case VK_LAUNCH_APP2:		break;
	case VK_OEM_1:				break; // Varies with keyboard. ; on US, $ on FR.
	case VK_OEM_PLUS:	return KeyCode::keyPlus;
	case VK_OEM_COMMA:	return KeyCode::keyComma;
	case VK_OEM_MINUS:	return KeyCode::keyMinus; // Doesn't work on FR layout?
	case VK_OEM_PERIOD:	return KeyCode::keyPeriod;
	case VK_OEM_2:				break; // Varies with keyboard layout. / on US, : on FR.
	case VK_OEM_3:				break; // Varies with keyboard layout. ` on US, ù on FR.
	case VK_OEM_4:				break; // Varies with keyboard layout.
	case VK_OEM_5:				break; // Varies with keyboard layout. \ on US.
	case VK_OEM_6:				break; // Varies with keyboard layout. ] on US, ^ on FR.
	case VK_OEM_7:				break; // Varies with keyboard layout. ' on US, ² on FR.
	case VK_OEM_8:				break; // Varies with keyboard layout. ! on FR.
	case VK_OEM_102:			break; // Varies with keyboard layout.
	case VK_PROCESSKEY:			break; // IME, unsupported.
	case VK_PACKET:				break; // Unicode.
	case VK_ATTN:				break;
	case VK_CRSEL:				break;
	case VK_EXSEL:				break;
	case VK_EREOF:				break;
	case VK_PLAY:				break;
	case VK_ZOOM:				break;
	case VK_NONAME:				LOG_DEBUG("VK_NONAME"); break;
	case VK_PA1:				break;
	case VK_OEM_CLEAR:			break;
	}
	return KeyCode::numberOfKeys;
}

#endif // ENABLE_KEYBOARD_AND_MOUSE

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((uMsg == WM_SYSCOMMAND && wParam == SC_SCREENSAVE) ||
		(uMsg == WM_SYSCOMMAND && wParam == SC_MONITORPOWER))
	{
		return 0;
	}

	if (uMsg == WM_CLOSE ||
		uMsg == WM_DESTROY ||
		(uMsg == WM_KEYDOWN && wParam == VK_ESCAPE))
	{
		PostQuitMessage(0);
		return 0;
	}

#ifdef ENABLE_KEYBOARD_AND_MOUSE
	if (uMsg == WM_CHAR)
	{
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_characterHandlers[i] != nullptr)
			{
				if (s_characterHandlers[i](wParam))
				{
					break;
				}
			}
		}
		return 0;
	}

	if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
	{
		const KeyCode::Enum keyCode = GetKeyCode(wParam);
		const bool keyPressed = (uMsg == WM_KEYDOWN);
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_keyHandlers[i] != nullptr)
			{
				if (s_keyHandlers[i](keyCode, keyPressed))
				{
					break;
				}
			}
		}
		return 0;
	}

	if (GetFocus() == hWnd)
	{
		POINT p;
		GetCursorPos(&p);
		int wheel = 0;
		if (uMsg == WM_MOUSEWHEEL)
		{
			wheel = GET_WHEEL_DELTA_WPARAM(wParam);
		}
		int left = GetAsyncKeyState(VK_LBUTTON);
		int right = GetAsyncKeyState(VK_RBUTTON);
		int middle = GetAsyncKeyState(VK_MBUTTON);
		int x1 = GetAsyncKeyState(VK_XBUTTON1);
		int x2 = GetAsyncKeyState(VK_XBUTTON2);

		RECT windowArea;
		GetWindowRect(hWnd, &windowArea);
		MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT) &windowArea, 2);

		if (p.x >= windowArea.left && p.x <= windowArea.right &&
			p.y >= windowArea.top && p.y <= windowArea.bottom)
		{
			// Conversion from screen coordinates to window coordinates:
			// https://msdn.microsoft.com/en-us/library/windows/desktop/dd162952(v=vs.85).aspx
			ScreenToClient(hWnd, &p);
			//p.x -= windowArea.left;
			//p.y -= windowArea.top;
	
			const bool leftDown = (left != 0);
			const bool rightDown = (right != 0);
			const bool middleDown = (middle != 0);
			const bool x1Down = (x1 != 0);
			const bool x2Down = (x2 != 0);
			for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
			{
				if (s_mouseHandlers[i] != nullptr)
				{
					if (s_mouseHandlers[i](p.x, p.y, wheel, leftDown, rightDown, middleDown, x1Down, x2Down))
					{
						break;
					}
				}
			}
		}
	}
#endif // ENABLE_KEYBOARD_AND_MOUSE

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Win32Platform::Win32Platform(const char* windowTitle,
							 int windowWidth, int windowHeight,
							 int monitorLeft, int monitorTop,
							 int monitorWidth, int monitorHeight,
							 bool fullscreen)
{
	DWORD			dwExStyle;
	DWORD			dwStyle;
	RECT			rec;
	WNDCLASSA		wc;

	m_hInstance = GetModuleHandle(0);

	memset(&wc, 0, sizeof(WNDCLASSA));
	wc.style			= CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= m_hInstance;
	wc.lpszClassName	= windowClassName;

	if (!RegisterClass((WNDCLASSA*)&wc))
	{
		Core::TerminateOnFatalError("Could not register window class.");
		return;
	}

	if (fullscreen)
	{
		dwExStyle	= WS_EX_APPWINDOW;
		dwStyle		= WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ShowCursor(0);
		rec.left	= monitorLeft;
		rec.top		= monitorTop;
	}
	else
	{
		dwExStyle	= WS_EX_APPWINDOW;// | WS_EX_WINDOWEDGE;
		dwStyle		= WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
		rec.left	= monitorLeft + ((monitorWidth - windowWidth) >> 1);
		rec.top		= monitorTop + ((monitorHeight - windowHeight) >> 1);
	}
	rec.right	= rec.left + windowWidth;
	rec.bottom	= rec.top + windowHeight;


	AdjustWindowRect(&rec, dwStyle, 0);
	m_hWnd = CreateWindowEx(dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
							rec.left, rec.top, rec.right - rec.left, rec.bottom - rec.top,
							0, 0, m_hInstance, 0);

	if (!m_hWnd)
	{
		Core::TerminateOnFatalError("Could not create window.");
		return;
	}

	SetWindowTitle(windowTitle);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	static const PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		8, 0, 8, 0, 8, 0, 8, 0,
		0, 0, 0, 0, 0,
		32,             // zbuffer bpp
		0,              // stencil bpp
		0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	if (!(m_hDC = GetDC(m_hWnd)))
	{
		Core::TerminateOnFatalError("Could not create drawing context.");
	}

	unsigned int PixelFormat;
	//wglChoosePixelFormatARB()
	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
	{
		Core::TerminateOnFatalError("Could not choose pixel format.");
	}

	if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		Core::TerminateOnFatalError("Could not set pixel format.");
	}

	// In theory we should call wglCreateContextAttribsARB to open an
	// OpenGL 3.x context. However just calling wglCreateContext seems
	// to work for now.
	//
	// The spec apparently says:
	//     The legacy context creation routines can only return OpenGL
	//     3.1 contexts if the GL_ARB_compatibility extension is
	//     supported, and can only return OpenGL 3.2 or greater
	//     contexts implementing the compatibility profile. This
	//     ensures compatibility for existing applications. However,
	//     3.0-aware applications are encouraged to use
	//     wglCreateContextAttribsARB instead of the legacy routines.
	//
	// https://www.opengl.org/discussion_boards/showthread.php/182941-wglCreateContextAttribsARB-not-needed-anymore
	//
	//  * * *
	//
	// Also, we want the "compatibility profile" and not the "core
	// profile", because wglUseFontBitmaps returns fonts as display
	// lists, which is a legacy OpenGL feature.
	//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd374392%28v=vs.85%29.aspx
	//
	//  * * *
	//
	// Finally, according to this slide from 2009, NVidia plans to stay
	// backward compatible. Their official statement has disappeared
	// since though.
	//
	// https://www.slideshare.net/Mark_Kilgard/opengl-32-and-more/36-Deprecation_NVIDIA_view_ulliWe_will
	// https://web.archive.org/web/20090826084131/http://developer.nvidia.com/object/opengl_3_driver.html
	//
	m_hRC = wglCreateContext(m_hDC);

	// For reference, here is the kind of code necessary to request a
	// compatibility profile:
#if 0
	wglMakeCurrent(m_hDC, m_hRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB)
	{
		wglMakeCurrent(0, 0);
		wglDeleteContext(m_hRC);

		int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		m_hRC = wglCreateContextAttribsARB(m_hDC, 0, attribList);
	}
#endif

	if (!m_hRC)
	{
		Core::TerminateOnFatalError("Could not create OpenGL rendering context.");
	}

	// Vertical sync, if available
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(1);
	}

	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		Core::TerminateOnFatalError("Could not use rendering context.");
	}
}

Win32Platform::~Win32Platform()
{
	if (m_hDC)
	{
		wglMakeCurrent(0, 0);
		wglDeleteContext(m_hRC);
		ReleaseDC(m_hWnd, m_hDC);
	}

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
	}

	UnregisterClass(windowClassName, m_hInstance);

	// Restore display settings and mouse cursor in case we changed them.
	ChangeDisplaySettings(0, 0);
	ShowCursor(1);
}

long Win32Platform::GetTime() const
{
	return timeGetTime();
}

void Win32Platform::Sleep(long milliseconds) const
{
	::Sleep(milliseconds);
}

void Win32Platform::SetWindowTitle(const char* title) const
{
	SetWindowText(m_hWnd, title);
}

#ifdef ENABLE_KEYBOARD_AND_MOUSE
void Win32Platform::AddInputHandler(CharacterHandler characterHandler, KeyHandler keyHandler, MouseHandler mouseHandler)
{
	assert(s_nextHandler < MAX_INPUT_HANDLERS);
	s_characterHandlers[s_nextHandler] = characterHandler;
	s_keyHandlers[s_nextHandler] = keyHandler;
	s_mouseHandlers[s_nextHandler] = mouseHandler;
	++s_nextHandler;
}
#endif // ENABLE_KEYBOARD_AND_MOUSE

bool Win32Platform::HandleMessages() const
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void Win32Platform::SwapBuffers() const
{
	::SwapBuffers(m_hDC);
}
