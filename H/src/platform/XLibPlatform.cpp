#define XLIB_PLATFORM_INCLUDE // See the end of XLibPlatform.hh for the explanation.
#include "XLibPlatform.hh"
#include "engine/core/Debug.hh"
#include <GL/glx.h>
#include <X11/keysym.h>
#include <ctime>
#include <stdio.h>

namespace
{
	GLint glAttributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	const long eventMask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
}

using namespace platform;

#ifdef ENABLE_KEYBOARD_AND_MOUSE

static ::platform::KeyCode::Enum GetKeyCode(KeySym ks)
{
	if (ks >= XK_F1 && ks <= XK_F12)
	{
		return (::platform::KeyCode::Enum)(::platform::KeyCode::keyF1 + (ks - XK_F1));
	}

	if (ks >= XK_KP_0 && ks <= XK_KP_9)
	{
		LOG_INFO("Keypad digit");
		return (::platform::KeyCode::Enum)(::platform::KeyCode::keyNumpad0 + (ks - XK_KP_0));
	}

	if (ks >= XK_0 && ks <= XK_9)
	{
		return (::platform::KeyCode::Enum)(::platform::KeyCode::key0 + (ks - XK_0));
	}

	if (ks >= XK_A && ks <= XK_Z)
	{
		return (::platform::KeyCode::Enum)(::platform::KeyCode::keyA + (ks - XK_A));
	}
	if (ks >= XK_a && ks <= XK_z)
	{
		return (::platform::KeyCode::Enum)(::platform::KeyCode::keyA + (ks - XK_a));
	}

	switch (ks)
	{
	case XK_Left:		return ::platform::KeyCode::keyArrowLeft;
	case XK_Right:		return ::platform::KeyCode::keyArrowRight;
	case XK_Up:			return ::platform::KeyCode::keyArrowUp;
	case XK_Down:		return ::platform::KeyCode::keyArrowDown;
	case XK_Prior:		return ::platform::KeyCode::keyPageUp;
	case XK_Next:		return ::platform::KeyCode::keyPageDown;

	case XK_Alt_L:		return ::platform::KeyCode::keyLeftAlt;
	case XK_Alt_R:		return ::platform::KeyCode::keyRightAlt;
	case XK_Control_L:	return ::platform::KeyCode::keyLeftControl;
	case XK_Control_R:	return ::platform::KeyCode::keyRightControl;
	case XK_Shift_L:	return ::platform::KeyCode::keyLeftShift;
	case XK_Shift_R:	return ::platform::KeyCode::keyRightShift;

	case XK_Return:		return ::platform::KeyCode::keyEnter;
	case XK_space:		return ::platform::KeyCode::keySpace;
	case XK_Tab:		return ::platform::KeyCode::keyTab;

	case XK_Print:		return ::platform::KeyCode::keyPrintScreen;
	case XK_Scroll_Lock:return ::platform::KeyCode::keyScrollLock;
	case XK_Pause:		return ::platform::KeyCode::keyPause;

	case XK_KP_Multiply:return ::platform::KeyCode::keyNumpadMultiply;
	case XK_KP_Add:		return ::platform::KeyCode::keyNumpadPlus;
	case XK_KP_Subtract:return ::platform::KeyCode::keyNumpadMinus;
	case XK_KP_Decimal:	return ::platform::KeyCode::keyNumpadDecimal;
	case XK_KP_Divide:	return ::platform::KeyCode::keyNumpadDivide;

	case XK_KP_Home:	return ::platform::KeyCode::keyNumpad7;
	case XK_KP_Left:	return ::platform::KeyCode::keyNumpad4;
	case XK_KP_Up:		return ::platform::KeyCode::keyNumpad8;
	case XK_KP_Right:	return ::platform::KeyCode::keyNumpad6;
	case XK_KP_Down:	return ::platform::KeyCode::keyNumpad2;
	case XK_KP_Prior:	return ::platform::KeyCode::keyNumpad9;
	case XK_KP_Next:	return ::platform::KeyCode::keyNumpad3;
	case XK_KP_End:		return ::platform::KeyCode::keyNumpad1;
	case XK_KP_Begin:	return ::platform::KeyCode::keyNumpad5;
	case XK_KP_Insert:	return ::platform::KeyCode::keyNumpad0;
	case XK_KP_Delete:	return ::platform::KeyCode::keyNumpadDecimal;
	}

	return ::platform::KeyCode::numberOfKeys;
}

#endif // ENABLE_KEYBOARD_AND_MOUSE

XLibPlatform::XLibPlatform(const char* windowTitle,
						   int width, int height,
						   int monitorLeft, int monitorTop,
						   int monitorWidth, int monitorHeight,
						   bool fullscreen)
{
	m_dpy = XOpenDisplay(NULL);
	if (m_dpy == NULL)
	{
		Core::TerminateOnFatalError("Could not connect to X server.");
		return;
	}

	const Window root = DefaultRootWindow(m_dpy);
	XVisualInfo* xvi = glXChooseVisual(m_dpy, 0, glAttributes);

	if (xvi == NULL)
	{
		Core::TerminateOnFatalError("Could not choose proper gl visual.");
		return;
	}
	else
	{
		LOG_INFO("Visual %p selected", (void*) xvi->visualid);
	}

	const Colormap cmap = XCreateColormap(m_dpy, root, xvi->visual,
										  AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;

	if (fullscreen)
	{
		swa.override_redirect = True;
	}

	m_win = XCreateWindow(m_dpy,
						  root,
						  0, // initial x (ignored)
						  0, // initial y (ignored)
						  width,
						  height,
						  0, // border (ignored)
						  xvi->depth,
						  InputOutput,
						  xvi->visual,
						  CWColormap | CWEventMask,
						  &swa);

	XMapWindow(m_dpy, m_win);
	XStoreName(m_dpy, m_win, windowTitle);

#if 0 // This code is not tested, so for now it's disabled.
	if (fullscreen)
	{
		// Written after this article:
		// http://tonyobryan.com/index.php?article=9

		struct motifHints {
			unsigned long	flags;
			unsigned long	functions;
			unsigned long	decorations;
			long			inputMode;
			unsigned long	status;
		};
		motifHints	hints;
		hints.flags = 2;        // Specify that we're changing the window decorations.
		hints.decorations = 0;  // 0 (false) means that window decorations should go bye-bye.

		Atom property = XInternAtom(m_dpy,"_MOTIF_WM_HINTS",True);
		XChangeProperty(m_dpy, m_win, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);

		XF86VidModeSwitchToMode(m_dpy, defaultscreen, video_mode);
		XF86VidModeSetViewPort(m_dpy, DefaultScreen, 0, 0);
		XMoveResizeWindow(m_dpy, m_win, 0, 0, width, height);
		XMapRaised(m_dpy, m_win);
		XGrabPointer(m_dpy, m_win, True, 0, GrabModeAsync, GrabModeAsync, m_win, 0L, CurrentTime);
		XGrabKeyboard(m_dpy, m_win, False, GrabModeAsync, GrabModeAsync, CurrentTime);
	}
#endif

	m_glc = glXCreateContext(m_dpy, xvi, NULL, GL_TRUE);
	glXMakeCurrent(m_dpy, m_win, m_glc);

	m_width = width;
	m_height = height;
}

XLibPlatform::~XLibPlatform()
{
	if (m_dpy == NULL)
	{
		return;
	}
	glXMakeCurrent(m_dpy, None, NULL);
	glXDestroyContext(m_dpy, m_glc);
	XDestroyWindow(m_dpy, m_win);
	XCloseDisplay(m_dpy);
}

void XLibPlatform::AddInputHandler(
	CharacterHandler characterHandler, KeyHandler keyHandler, MouseHandler mouseHandler)
{
#ifdef ENABLE_KEYBOARD_AND_MOUSE
	m_handleKey = keyHandler;
	m_handleMouse = mouseHandler;
#endif // ENABLE_KEYBOARD_AND_MOUSE
}

int XLibPlatform::GetWidth() const
{
	return m_width;
}

int XLibPlatform::GetHeight() const
{
	return m_height;
}

long XLibPlatform::GetTime() const
{
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void XLibPlatform::Sleep(long milliseconds) const
{
	struct timeval tv;
	tv.tv_usec = (milliseconds % 1000) * 1000;
	tv.tv_sec = milliseconds / 1000;
	select(0, 0, 0, 0, &tv);
}

void XLibPlatform::SetWindowTitle(const char* title) const
{
	XStoreName(m_dpy, m_win, title);
}

bool XLibPlatform::HandleMessages() const
{
	bool keepRunning = true;
	int wheelSpeed = 50;

	XEvent xev;
	XSelectInput(m_dpy, m_win, eventMask);
	while (XPending(m_dpy))
	{
		XNextEvent(m_dpy, &xev);
		if (xev.type == KeyPress || xev.type == KeyRelease)
		{
			KeySym ks = XLookupKeysym((XKeyEvent*)&xev, 0);
			if (ks == XK_Escape)
			{
				keepRunning = false;
			}
#ifdef ENABLE_KEYBOARD_AND_MOUSE
			else if (m_handleKey != NULL)
			{
				m_handleKey(GetKeyCode(ks), xev.type == KeyPress);
			}
		}
		else if (m_handleMouse != NULL)
		{
			if (xev.type == ButtonPress)
			{
				XButtonPressedEvent* mouse = (XButtonPressedEvent*)&xev;
				int wheel = mouse->button == 4 ? 1 : (mouse->button == 5 ? -1 : 0);
				wheel *= wheelSpeed;
				m_handleMouse(mouse->x, mouse->y, wheel, mouse->button == 1,
							  mouse->button == 3, mouse->button == 2, false, false);
				// break here, so that the demo can be updated before receiving
				// the ButtonRelease event.
				break;
			}
			else if (xev.type == ButtonRelease)
			{
				XButtonReleasedEvent* mouse = (XButtonReleasedEvent*)&xev;
				m_handleMouse(mouse->x, mouse->y, 0, false, false, false, false, false);
			}
			else if (xev.type == MotionNotify)
			{
				XMotionEvent* mouse = (XMotionEvent*)&xev;
				int wheel = (mouse->state & Button4Mask) ? 1 : (mouse->state & Button5Mask ? -1 : 0);
				wheel *= wheelSpeed;
				m_handleMouse(mouse->x, mouse->y, wheel, mouse->state & Button1Mask,
							  mouse->state & Button3Mask, mouse->state & Button2Mask, false, false);
			}
#endif // ENABLE_KEYBOARD_AND_MOUSE
		}
	}

	return keepRunning;
}

void XLibPlatform::SwapBuffers() const
{
	glXSwapBuffers( m_dpy, m_win);
}
