#include "dialog.hh"
#include "sys/msys.h"

struct Resolution
{
  int w;
  int h;
  bool isValid;
  const char* text;
};

static Resolution resolutions[] = {
  { 800,  600, false, "800x600 (4/3)"},
  {1024,  768, false, "1024x768 (4/3)"},
  {1200,  900, false, "1200x900 (4/3)"},
  {1280,  720, false, "1280x720 (16/9)"},
  {1280,  800, false, "1280x800 (16/10)"},
  {1280, 1024, false, "1280x1024 (5/4)"},
  {1368,  768, false, "1368x768"},
  {1440,  900, false, "1440x900 (16/10)"},
  {1600, 1200, false, "1600x1200 (4/3)"},
  {1680, 1050, false, "1680x1050 (16/10)"},
  {1920, 1080, false, "1920x1080 (16/9)"},
  {1920, 1200, false, "1920x1200 (16/10)"},
//   { 960,   89, false, "Main Party huge screen"},
};


Dialog * Dialog::instance_;


Dialog::Dialog()
  : test_(false), fullscreen_(false), sound_(false),
    width_(0), height_(0), ids_(0)
{
}

void
Dialog::run()
{
  MSG msg;

  form_register();
  form_init();

  ShowWindow(form_, SW_SHOW);
  UpdateWindow(form_);
  SetFocus(button_ok_);

  while (GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
}

void
Dialog::form_register()
{
  WNDCLASS wc;

  hinstance_ = GetModuleHandle(0);

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc   = input;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName  = 0;
  wc.lpszClassName = "CtrlAltTest-setup";
  wc.hInstance     = hinstance_;

  ATOM res = RegisterClass(&wc);
  assert(res != 0);
  // TODO: error handling
}

void
Dialog::form_init()
{
  DWORD style;
  DWORD exstyle;
  RECT rect;

  style    = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
  exstyle  = WS_EX_APPWINDOW;

  rect.left   = 0;
  rect.right  = 400;
  rect.top    = 0;
  rect.bottom = 140;
  AdjustWindowRectEx(&rect, style, false, exstyle);

  form_ = CreateWindowEx(exstyle, "CtrlAltTest-setup", "B - Incubation, by Ctrl-Alt-Test", style,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 rect.right  - rect.left,
			 rect.bottom - rect.top,
			 0, 0, hinstance_, 0);

  button_ok_ = button_create("Test", 305, 102, 80, 28, id_button_ok_);
  button_ko_ = button_create("Don't test",  210, 102, 80, 28, id_button_ko_);
//   checkbox_gamma_  = checkbox_create("Gamma correction (video projector mode)", 15, 46, 325, 28, true, id_checkbox_gamma_);
  checkbox_fs_     = checkbox_create("Fullscreen", 15, 10, 100, 28, true, id_checkbox_fs_);
  checkbox_sound_  = checkbox_create("Music",      15, 46, 100, 28, true, id_checkbox_sound_);

  combobox_create(140, 10, 200, 250);

//  if (!form_)
// TODO: error handling
}


HWND
Dialog::button_create (const char* text, int x, int y, int w, int h, int &id)
{
  HWND res;

  res = CreateWindow("BUTTON", text,
		     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		     x, y, w, h, form_, (HMENU) ++ids_,
		     (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);

  id = ids_;
  return res;
}

HWND
Dialog::checkbox_create (const char* text,
			 int x, int y,
			 int w, int h,
			 bool checked, int &id)
{
  HWND res;

  res = CreateWindow("BUTTON", text,
		     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		     x, y, w, h, form_, (HMENU) ++ids_,
		     (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);
  CheckDlgButton(form_, ids_, checked ? BST_CHECKED : BST_UNCHECKED);

  id = ids_;
  return res;
}


void
Dialog::combobox_create (int x, int y, int w, int h)
{
  bool mine = false;
  int inserted = 0;
  int width  = GetSystemMetrics(0);
  int height = GetSystemMetrics(1);

  combobox_resolutions_ =
    CreateWindow("COMBOBOX", "",
		 WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL,
		 x, y, w, h, form_, (HMENU) ++ids_,
		 (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);
  id_combobox_resolutions_ = ids_;

  const int numberOfResolutions = sizeof(resolutions) / sizeof(Resolution);
  for (int r = 0; r < numberOfResolutions; ++r)
    if (resolutions[r].w <= width &&
	resolutions[r].h <= height)
    {
      resolutions[r].isValid = true;
      mine |= (resolutions[r].w == width && resolutions[r].h == height);
      inserted += 1;
      SendMessage(combobox_resolutions_, CB_ADDSTRING,
		  0, (LPARAM) resolutions[r].text);
    }

  if (!mine)
  {
    inserted += 1;
    SendMessage(combobox_resolutions_, CB_ADDSTRING,
		0, (LPARAM) "Current screen resolution");
  }

  SendMessage(combobox_resolutions_, CB_SETCURSEL, (WPARAM) inserted - 1, 0);
}


LRESULT CALLBACK
Dialog::input(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
  int wmi;
  int wme;

  const int numberOfResolutions = sizeof(resolutions) / sizeof(Resolution);

  switch (msg)
  {
    case WM_COMMAND:
      wmi = LOWORD(wp);
      wme = HIWORD(wp);

      if (wmi == instance_->id_button_ok_)
      {
	LRESULT index;

	index = SendMessage(instance_->combobox_resolutions_, CB_GETCURSEL, 0, 0);

	instance_->test_       = true;
	instance_->fullscreen_ = instance_->is_checked(instance_->id_checkbox_fs_);
	instance_->sound_      = instance_->is_checked(instance_->id_checkbox_sound_);
// 	instance_->gamma_      = instance_->is_checked(instance_->id_checkbox_gamma_);

	// Récupérer la résolution
	int i = 0;
	int current = 0;
	while (i < numberOfResolutions)
	{
	  if (resolutions[i].isValid)
	    if (current++ == index)
	    {
	      break;
	    }
	  ++i;
	}
	if (numberOfResolutions == i)
	{
	  instance_->width_ = GetSystemMetrics(0);
	  instance_->height_ = GetSystemMetrics(1);
	}
	else
	{
	  instance_->width_ = resolutions[i].w;
	  instance_->height_ = resolutions[i].h;
	}
      }

      if (wmi == instance_->id_button_ok_ ||
	  wmi == instance_->id_button_ko_)
      {
	DestroyWindow(instance_->form_);
	return 0;
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(h, msg, wp, lp);
}
