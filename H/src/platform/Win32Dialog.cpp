#include "Win32Dialog.hh"

#include "engine/algebra/Functions.hh"
#include "engine/container/Algorithm.hxx"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/StringUtils.hh"

// Set to 1 to get a list of device supported resolutions.
// Set to 0 to always use native resolution, but choose the rendering
// resolution instead.
#define ENABLE_LEGACY_RESOLUTION_MENU 0

// Set to 1 to launch the demo on the display the dialog box is on.
// Set to 0 to always launch the demo on the primary display.
#define ENABLE_MULTIPLE_MONITOR_SUPPORT 1

using namespace platform;

struct RatioDesc
{
	char w;
	char h;
	const char* desc;
};

// List of typical display aspect ratios found on the market.
static RatioDesc ratios[] = {
	{  1,   1, "1:1 (square)"},
	{  4,   3, "4:3"},
	{ 16,  10, "16:10"},
	{ 16,   9, "16:9"},
	{ 19,  10, "19:10"},
	{ 21,   9, "21:9 (2.33:1)"},
};

Dialog * Dialog::s_instance;

Dialog::Dialog():
	isOk(false),
	fullscreen(false),
	m_ids(0)
{
}

void Dialog::run(const char* title)
{
	const char* className = "CtrlAltTest-setup";

#define DEFAULT_DPI 96
	::SetProcessDPIAware();
	dpi = ::GetDeviceCaps(GetDC(NULL), LOGPIXELSX);

	m_hInstance = GetModuleHandle(0);

	WNDCLASS wc = {
		CS_OWNDC,
		input,
		0,
		0,
		m_hInstance,
		0,
		0,
		(HBRUSH)COLOR_WINDOW,
		0,
		className,
	};

	ATOM res = RegisterClass(&wc);
	assert(res != 0);
	// TODO: error handling

	DWORD style;
	DWORD exstyle;

	style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	exstyle = WS_EX_APPWINDOW;

	const int desiredWidth = (400*dpi)/DEFAULT_DPI;
	const int desiredHeight = (200*dpi)/DEFAULT_DPI;

	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);

	RECT dialogRect;
	dialogRect.left = (desktopRect.right - desiredWidth) / 2;
	dialogRect.top = (desktopRect.bottom - desiredHeight) / 2;
	dialogRect.right  = dialogRect.left + desiredWidth;
	dialogRect.bottom = dialogRect.top + desiredHeight;
	AdjustWindowRectEx(&dialogRect, style, false, exstyle);

	m_hDlg = CreateWindowEx(exstyle, className, title, style,
							dialogRect.left, dialogRect.top,
							dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top,
							0, 0, m_hInstance, 0);

	// Documentation about GetStockObject(DEFAULT_GUI_FONT):
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925%28v=vs.85%29.aspx
	//
	// "It is not recommended that you employ this method to obtain the
	// current font used by dialogs and windows. Instead, use the
	// SystemParametersInfo function with the SPI_GETNONCLIENTMETRICS
	// parameter to retrieve the current font. SystemParametersInfo
	// will take into account the current theme and provides font
	// information for captions, menus, and message dialogs."

	// Code snippet from:
	// http://stackoverflow.com/questions/2942828/correct-dialog-ui-font-on-windows/2942869#2942869
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &metrics, 0);
	m_hFont = CreateFontIndirect(&metrics.lfMessageFont);

	SendMessage(m_hDlg, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	const int vSpace = (30*dpi)/DEFAULT_DPI;
	const int x = (30*dpi)/DEFAULT_DPI;
	int y = vSpace;
	const int w1 = (150*dpi)/DEFAULT_DPI;
	const int w2 = (150*dpi)/DEFAULT_DPI;
	const int h1 = (28*dpi)/DEFAULT_DPI;
	const int h2 = (130*dpi)/DEFAULT_DPI;
	m_combobox_resolutions = combobox_create("Resolution:", x, y, w1, w2, h2, &m_id_combobox_resolutions);
	fill_resolutions_combobox();

	y += vSpace;
	m_combobox_ratios = combobox_create("Display aspect ratio:", x, y, w1, w2, h2, &m_id_combobox_ratios);
	fill_ratios_combobox();

#if ENABLE_QUALITY_OPTION
	y += vSpace;
	m_combobox_qualities = combobox_create("Graphics:", x, y, w1, w2, h2, &m_id_combobox_qualities);
	fill_qualities_combobox();
#endif // ENABLE_QUALITY_OPTION

	y += vSpace;
	m_checkbox_fs = checkbox_create("Fullscreen", (185*dpi)/DEFAULT_DPI, y, (110*dpi)/DEFAULT_DPI, h1, true, &m_id_checkbox_fs);

	y += vSpace;
	m_button_ok = button_create("Test", (120*dpi)/DEFAULT_DPI, y, w1, h1, &m_id_button_ok);

	//  if (!m_hDlg)
	// TODO: error handling

	ShowWindow(m_hDlg, SW_SHOW);
	UpdateWindow(m_hDlg);
	SetFocus(m_button_ok);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

HWND Dialog::button_create(const char* name,
						   int x, int y,
						   int w, int h,
						   int* id)
{
	HWND res =
		CreateWindow("BUTTON", name,
					 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
					 x, y, w, h, m_hDlg, (HMENU)++m_ids,
					 (HINSTANCE)GetWindowLong(m_hDlg, GWL_HINSTANCE), 0);
	SendMessage(res, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	*id = m_ids;
	return res;
}

HWND Dialog::checkbox_create(const char* name,
							 int x, int y,
							 int w, int h,
							 bool isChecked,
							 int* id)
{
	HWND res =
		CreateWindow("BUTTON", name,
					 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
					 x, y, w, h, m_hDlg, (HMENU)++m_ids,
					 (HINSTANCE)GetWindowLong(m_hDlg, GWL_HINSTANCE), 0);
	SendMessage(res, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	CheckDlgButton(m_hDlg, m_ids, (isChecked ? BST_CHECKED : BST_UNCHECKED));

	*id = m_ids;
	return res;
}

HWND Dialog::combobox_create(const char* name,
							 int x, int y,
							 int w1, int w2,
							 int h,
							 int* id)
{
	HWND label =
		CreateWindow("STATIC", name,
					 WS_CHILD | WS_VISIBLE | SS_RIGHT,
					 x, y + 5, w1, h,
					 m_hDlg, (HMENU)++m_ids,
					 (HINSTANCE)GetWindowLong(m_hDlg, GWL_HINSTANCE), 0);
	SendMessage(label, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	HWND res =
		CreateWindow("COMBOBOX", NULL,
					 WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL,
					 x + w1 + 25, y, w2, h, m_hDlg, (HMENU)++m_ids,
					 (HINSTANCE)GetWindowLong(m_hDlg, GWL_HINSTANCE), 0);
	SendMessage(res, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	*id = m_ids;

	return res;
}

struct ResolutionDesc
{
	unsigned long w;
	unsigned long h;

	inline bool operator == (const ResolutionDesc& rhs) const
	{
		return (w == rhs.w) && (h == rhs.h);
	}

	inline bool operator > (const ResolutionDesc& rhs) const
	{
		return (w > rhs.w) || (w == rhs.w && h > rhs.h);
	}
};

Container::Array<ResolutionDesc> s_resolutions;

#if ENABLE_LEGACY_RESOLUTION_MENU
static void DetectResolutions(Container::Array<ResolutionDesc>* resolutions, int maxResolutions)
{
	// Not initialized on purpose: it's a big structure that will
	// generate a memset call.
	DEVMODE dm;

	dm.dmSize = sizeof(dm);
	for (int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; ++i)
	{
		ResolutionDesc newRes = { dm.dmPelsWidth, dm.dmPelsHeight };
		if (Container::find(*resolutions, newRes) < 0 && resolutions->size < maxResolutions)
		{
			resolutions->add(newRes);
		}
	}
#ifdef ENABLE_SORT_RESOLUTIONS
	Container::bubbleSort(*resolutions);
#endif
}

void Dialog::fill_resolutions_combobox()
{
	bool isCurrentResolutionIncluded = false;
	int indexOfCurrentResolution = 0;
	const ResolutionDesc currentResolution = {
		(unsigned long)GetSystemMetrics(SM_CXSCREEN),
		(unsigned long)GetSystemMetrics(SM_CYSCREEN),
	};

	const int maxResolutions = 100; // "Ought to be enough for anyone."
	s_resolutions.init(100);
	DetectResolutions(&s_resolutions, maxResolutions);

	for (int i = 0; i < s_resolutions.size; ++i)
	{
		const ResolutionDesc& resolution = s_resolutions[i];
		if (resolution == currentResolution)
		{
			isCurrentResolutionIncluded = true;
			indexOfCurrentResolution = i;
		}

		char description[5+1+5+1]; // Five digits each should be enough.
		char * p = Core::itoa(resolution.w, description);
		*p++ = 'x';
		Core::itoa(resolution.h, p);

		SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)description);
	}
	if (!isCurrentResolutionIncluded)
	{
		SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"Current resolution");
		indexOfCurrentResolution = s_resolutions.size;
	}

	SendMessage(m_combobox_resolutions, CB_SETCURSEL, (WPARAM)indexOfCurrentResolution, 0);
}

#else // !ENABLE_LEGACY_RESOLUTION_MENU

void Dialog::fill_resolutions_combobox()
{
	int nativeWidth = GetSystemMetrics(SM_CXSCREEN);
	int nativeHeight = GetSystemMetrics(SM_CYSCREEN);

	s_resolutions.init(8);

	// SIZE: To support more than 3 options other than native, a loop
	// becomes more size efficient.
#if 0
	const int resolutions[] = { 480, 720, 1080, 1440, nativeHeight };
	const char* descriptions[] = { "480p", "720p", "1080p", "1440p", "Native" };
	for (unsigned int i = 0; i < ARRAY_LEN(resolutions); ++i)
	{
		int vRes = resolutions[i];
		if (nativeHeight > vRes || i == ARRAY_LEN(resolutions) - 1)
		{
			SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)descriptions[i]);
			ResolutionDesc newRes = { (vRes * nativeWidth) / nativeHeight, vRes };
			s_resolutions.add(newRes);
		}
	}
#else
	//if (nativeHeight > 480)
	//{
	//	SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"480p");
	//	ResolutionDesc newRes = { (480 * nativeWidth) / nativeHeight, 480 };
	//	s_resolutions.add(newRes);
	//}
	if (nativeHeight > 720)
	{
		SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"720p");
		ResolutionDesc newRes = { (720 * nativeWidth) / nativeHeight, 720 };
		s_resolutions.add(newRes);
	}
	if (nativeHeight > 1080)
	{
		SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"1080p");
		ResolutionDesc newRes = { (1080 * nativeWidth) / nativeHeight, 1080 };
		s_resolutions.add(newRes);
	}
	//if (nativeHeight > 1440)
	//{
	//	SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"1440p");
	//	ResolutionDesc newRes = { (1440 * nativeWidth) / nativeHeight, 1440 };
	//	s_resolutions.add(newRes);
	//}
	{
		SendMessage(m_combobox_resolutions, CB_ADDSTRING, 0, (LPARAM)"Native");
		ResolutionDesc newRes = { nativeWidth, nativeHeight };
		s_resolutions.add(newRes);
	}
#endif

	SendMessage(m_combobox_resolutions, CB_SETCURSEL, (WPARAM)s_resolutions.size - 1, 0);
}
#endif // !ENABLE_LEGACY_RESOLUTION_MENU

static int guessAspectRatio(int width, int height)
{
	const float ratio = float(width) / float(height);
	int bestGuess = 0;
	float bestError = Algebra::abs(ratio - float(ratios[bestGuess].w) / float(ratios[bestGuess].h));
	for (size_t i = 0; i < ARRAY_LEN(ratios); ++i)
	{
		float error = Algebra::abs(ratio - float(ratios[i].w) / float(ratios[i].h));
		if (error < bestError)
		{
			bestGuess = i;
			bestError = error;
		}
	}
	return bestGuess;
}

void Dialog::fill_ratios_combobox()
{
	int width  = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	for (size_t i = 0; i < ARRAY_LEN(ratios); ++i)
	{
		SendMessage(m_combobox_ratios, CB_ADDSTRING, 0, (LPARAM)ratios[i].desc);
	}

	int guessedRatio = guessAspectRatio(width, height);
	SendMessage(m_combobox_ratios, CB_SETCURSEL, (WPARAM)guessedRatio, 0);
}

#if ENABLE_QUALITY_OPTION
void Dialog::fill_qualities_combobox()
{
	static char* qualities[] = { "Low", "Medium", "High" };
	for (size_t i = 0; i < ARRAY_LEN(qualities); ++i)
	{
		SendMessage(m_combobox_qualities, CB_ADDSTRING, 0, (LPARAM)qualities[i]);
	}
	SendMessage(m_combobox_qualities, CB_SETCURSEL, (WPARAM)(ARRAY_LEN(qualities) - 1), 0);
}
#endif // ENABLE_QUALITY_OPTION

inline bool isChecked(HWND hDlg, int cb)
{
  return IsDlgButtonChecked(hDlg, cb) == BST_CHECKED;
}

LRESULT CALLBACK Dialog::input(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
	int wmi;
	int wme;

	switch (msg)
	{
	case WM_COMMAND:
		wmi = LOWORD(wp);
		wme = HIWORD(wp);

		if (wmi == s_instance->m_id_button_ok)
		{
			LRESULT resolutionIndex = SendMessage(s_instance->m_combobox_resolutions, CB_GETCURSEL, 0, 0);
			LRESULT ratioIndex = SendMessage(s_instance->m_combobox_ratios, CB_GETCURSEL, 0, 0);

#if ENABLE_QUALITY_OPTION
			LRESULT qualityIndex = SendMessage(s_instance->m_combobox_qualities, CB_GETCURSEL, 0, 0);
			s_instance->quality = qualityIndex;
#if DEBUG
			s_instance->quality += 2; // see Settings.hh; skip RidiculouslyLow and VeryLow
#endif
#endif	// ENABLE_QUALITY_OPTION
			s_instance->isOk = true;
			s_instance->fullscreen = isChecked(s_instance->m_hDlg, s_instance->m_id_checkbox_fs);

			// Resolution:
#if ENABLE_MULTIPLE_MONITOR_SUPPORT
			HMONITOR monitor = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(monitor, &monitorInfo);
			s_instance->monitorLeft = monitorInfo.rcMonitor.left;
			s_instance->monitorTop = monitorInfo.rcMonitor.top;
			s_instance->monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			s_instance->monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
#else // !ENABLE_MULTIPLE_MONITOR_SUPPORT
			s_instance->monitorLeft = 0;
			s_instance->monitorTop = 0;
			s_instance->monitorWidth = GetSystemMetrics(SM_CXSCREEN);
			s_instance->monitorHeight = GetSystemMetrics(SM_CYSCREEN);
#endif // !ENABLE_MULTIPLE_MONITOR_SUPPORT

			s_instance->renderWidth = 0;
			s_instance->renderHeight = 0;
			int current = 0;
			for (int i = 0; i < s_resolutions.size; ++i)
			{
				if (current++ == resolutionIndex)
				{
					s_instance->renderWidth = s_resolutions[i].w;
					s_instance->renderHeight = s_resolutions[i].h;
#if ENABLE_LEGACY_RESOLUTION_MENU
					s_instance->monitorWidth = s_resolutions[i].w;
					s_instance->monitorHeight = s_resolutions[i].h;
#endif // ENABLE_LEGACY_RESOLUTION_MENU
					break;
				}
			}

			// Aspect ratio:
			s_instance->displayAspectRatio = float(ratios[ratioIndex].w) / float(ratios[ratioIndex].h);

			DestroyWindow(s_instance->m_hDlg);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(h, msg, wp, lp);
}
