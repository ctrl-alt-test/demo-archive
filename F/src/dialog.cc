#include "dialog.hh"
#include "sys/msys.h"
#include "intro.hh"

struct ResolutionDesc
{
  short w;
  short h;
  char guessedRatio;
  bool isValid;
  const char* desc;
};

static ResolutionDesc resolutions[] = {
  { 800,  600, 0, false, "800x600"},
  {1024,  768, 0, false, "1024x768"},
  {1200,  900, 0, false, "1200x900"},
  {1280,  720, 1, false, "1280x720"},
  {1280,  800, 2, false, "1280x800"},
  {1280, 1024, 0, false, "1280x1024"},
  {1368,  768, 1, false, "1368x768"},
  {1440,  900, 2, false, "1440x900"},
  {1600, 1200, 0, false, "1600x1200"},
  {1680, 1050, 2, false, "1680x1050"},
  {1920, 1080, 1, false, "1920x1080"},
  {1920, 1200, 2, false, "1920x1200"},
//   { 960,   89, 3, false, "La Grande Halle's huge screen"},
};

struct RatioDesc
{
  char w;
  char h;
  const char* desc;
};

static RatioDesc ratios[] = {
  { 4,  3, "4:3"},
  {16,  9, "16:9"},
  {16, 10, "16:10"},
//   {32,  3, "La Grande Halle's huge screen"},
};


Dialog * Dialog::instance_;


Dialog::Dialog():
  test_(false),
  fullscreen_(false),
  width_(0),
  height_(0),
  ratio_(1.f),
  ids_(0)
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

  form_ = CreateWindowEx(exstyle, "CtrlAltTest-setup", DEMO_TITLE ", by Ctrl-Alt-Test", style,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 rect.right  - rect.left,
			 rect.bottom - rect.top,
			 0, 0, hinstance_, 0);

  combobox_resolutions_ = combobox_create("Resolution:", 40, 10, 100, 150, 250, id_combobox_resolutions_);
  int guessedRatio = 0;
  fill_resolutions_combobox(guessedRatio);

  combobox_ratios_ = combobox_create("Monitor aspect:", 40, 40, 100, 150, 250, id_combobox_ratios_);
  fill_ratios_combobox(guessedRatio);

  checkbox_fs_ = checkbox_create("Fullscreen", 165, 70, 110, 28, true, id_checkbox_fs_);

//   checkbox_gamma_  = checkbox_create("Gamma correction (video projector mode)", 15, 46, 325, 28, true, id_checkbox_gamma_);

  button_ok_ = button_create("Test",        290, 102, 90, 28, id_button_ok_);
  //button_ko_ = button_create("Don't test",  190, 102, 90, 28, id_button_ko_);

//  if (!form_)
// TODO: error handling
}


HWND
Dialog::button_create (const char* name, int x, int y, int w, int h, int &id)
{
  HWND res =
    CreateWindow("BUTTON", name,
		 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		 x, y, w, h, form_, (HMENU) ++ids_,
		 (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);

  id = ids_;
  return res;
}

HWND
Dialog::checkbox_create (const char* name,
			 int x, int y,
			 int w, int h,
			 bool checked, int &id)
{
  HWND res =
    CreateWindow("BUTTON", name,
		 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		 x, y, w, h, form_, (HMENU) ++ids_,
		 (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);
  CheckDlgButton(form_, ids_, checked ? BST_CHECKED : BST_UNCHECKED);

  id = ids_;
  return res;
}


HWND
Dialog::combobox_create(const char* name, int x, int y, int w1, int w2, int h, int &id)
{
  HWND label = CreateWindow("STATIC", name,
			    WS_CHILD | WS_VISIBLE | SS_RIGHT,
			    x, y + 5, w1, h,
			    form_, (HMENU) ++ids_,
			    (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);

  HWND res =
    CreateWindow("COMBOBOX", NULL,
		 WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL,
		 x + w1 + 25, y, w2, h, form_, (HMENU) ++ids_,
		 (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);
  id = ids_;

  return res;
}

void
Dialog::fill_resolutions_combobox(int & guessedRatio)
{
  bool mineIncluded = false;
  int mine = 0;
  int inserted = 0;
  int width  = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);
  const int numberOfResolutions = sizeof(resolutions) / sizeof(ResolutionDesc);
  for (int r = 0; r < numberOfResolutions; ++r)
    if (resolutions[r].w <= width &&
	resolutions[r].h <= height)
    {
      resolutions[r].isValid = true;
      if (resolutions[r].w == width && resolutions[r].h == height)
      {
	mineIncluded = true;
	mine = inserted;
	guessedRatio = (int)resolutions[r].guessedRatio;
      }
      inserted += 1;
      SendMessage(combobox_resolutions_, CB_ADDSTRING, 0, (LPARAM) resolutions[r].desc);
    }

  if (!mineIncluded)
  {
    inserted += 1;
    SendMessage(combobox_resolutions_, CB_ADDSTRING, 0, (LPARAM) "Current screen resolution");
  }

  SendMessage(combobox_resolutions_, CB_SETCURSEL, (WPARAM) mine, 0);
}


void
Dialog::fill_ratios_combobox(int guessedRatio)
{
  int width  = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);

  const int numberOfRatios = sizeof(ratios) / sizeof(RatioDesc);
  for (int r = 0; r < numberOfRatios; ++r)
  {
    SendMessage(combobox_ratios_, CB_ADDSTRING, 0, (LPARAM) ratios[r].desc);
  }

  SendMessage(combobox_ratios_, CB_SETCURSEL, (WPARAM) guessedRatio, 0);
}


LRESULT CALLBACK
Dialog::input(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
  int wmi;
  int wme;

  const int numberOfResolutions = sizeof(resolutions) / sizeof(ResolutionDesc);

  switch (msg)
  {
    case WM_COMMAND:
      wmi = LOWORD(wp);
      wme = HIWORD(wp);

      if (wmi == instance_->id_button_ok_)
      {
	LRESULT resolutionIndex =
	  SendMessage(instance_->combobox_resolutions_, CB_GETCURSEL, 0, 0);
	LRESULT ratioIndex =
	  SendMessage(instance_->combobox_ratios_, CB_GETCURSEL, 0, 0);

	instance_->test_       = true;
	instance_->fullscreen_ = instance_->is_checked(instance_->id_checkbox_fs_);
// 	instance_->gamma_      = instance_->is_checked(instance_->id_checkbox_gamma_);

	// Résolution
	instance_->width_ = GetSystemMetrics(SM_CXSCREEN);
	instance_->height_ = GetSystemMetrics(SM_CYSCREEN);
	int current = 0;
	for (int i = 0; i < numberOfResolutions; ++i)
	  if (resolutions[i].isValid)
	    if (current++ == resolutionIndex)
	    {
	      instance_->width_ = resolutions[i].w;
	      instance_->height_ = resolutions[i].h;
	      break;
	    }

	// Aspect ratio
	if (instance_->fullscreen_)
	  instance_->ratio_ = float(ratios[ratioIndex].w) / float(ratios[ratioIndex].h);
	else
	  instance_->ratio_ = float(instance_->width_) / float(instance_->height_);

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
