#include "dialog.hxx"

#include "array.hxx"
#include "intro.hh"

#include "sys/msys_libc.h"

//
// FIXME : deux fonctions à mettre dans un Utils quand on en aura un
//
void reverse(char str[], int length)
{
  int start = 0;
  int end = length -1;
  while (start < end)
  {
    char tmp = str[start];
    str[start] = str[end];
    str[end] = tmp;
    start++;
    end--;
  }
}

// Renvoie la position _après_ le nombre
char* itoa(unsigned long num, char* str)
{
  int i = 0;
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  while (num != 0)
  {
    char rem = (char)(num % 10);
    str[i++] = rem + '0';
    num = num / 10;
  }
  str[i] = '\0';
  reverse(str, i);
  return str + i;
}

struct ResolutionDesc
{
  unsigned long w;
  unsigned long h;
  char desc[12];

  void createDesc()
  {
    char * p = itoa(w, desc);
    *p++ = 'x';
    itoa(h, p);
  }

  inline bool operator == (const ResolutionDesc & other) const
  { return w == other.w && h == other.h; }

  inline bool operator > (const ResolutionDesc & other) const
  { return w > other.w || (w == other.w && h > other.h); }
};

Array<ResolutionDesc> resolutions;

static void DetectResolutions()
{
  const int max = 100; // "Ought to be enough for anyone."
  resolutions.init(max);

  DEVMODE dm; // On n'initialise pas : la struct est grosse, ça génère un appel à memset
  dm.dmSize = sizeof(dm);
  for (int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; ++i)
  {
    ResolutionDesc newRes = { dm.dmPelsWidth, dm.dmPelsHeight };
    if (resolutions.find(newRes) < 0 && resolutions.size < max)
    {
      newRes.createDesc();
      resolutions.add(newRes);
    }
  }
  resolutions._quickSort(0, resolutions.size - 1);
}

struct RatioDesc
{
  char w;
  char h;
  const char* desc;
};

// Liste de résolutions de moniteurs qu'on trouve sur le marché
static RatioDesc ratios[] = {
  {  4,   3, "4:3"},
  { 16,  10, "16:10"},
  { 16,   9, "16:9"},
  { 19,  10, "19:10"},
  { 21,   9, "21:9 (2.33:1)"},
//   {32,  3, "La Grande Halle's huge screen"},
};


Dialog * Dialog::instance_;


Dialog::Dialog():
  test_(false),
  fullscreen_(false),
  credits_(false),
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
  rect.bottom = 170;
  AdjustWindowRectEx(&rect, style, false, exstyle);

  form_ = CreateWindowEx(exstyle, "CtrlAltTest-setup", DEMO_TITLE ", by Ctrl-Alt-Test and hAND", style,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 rect.right  - rect.left,
			 rect.bottom - rect.top,
			 0, 0, hinstance_, 0);

  int y = 10;
  combobox_resolutions_ = combobox_create("Resolution:", 60, y, 100, 150, 130, id_combobox_resolutions_);
  fill_resolutions_combobox();

  y += 30;
  combobox_ratios_ = combobox_create("Screen aspect:", 60, y, 100, 150, 130, id_combobox_ratios_);
  fill_ratios_combobox();

  y += 30;
  checkbox_fs_ = checkbox_create("Fullscreen", 185, y, 110, 28, true, id_checkbox_fs_);

  y += 30;
//   checkbox_gamma_  = checkbox_create("Gamma correction (video projector mode)", 15, 46, 325, 28, true, id_checkbox_gamma_);
  checkbox_nocredit_ = checkbox_create("Hide credits", 185, y, 110, 28, false, id_checkbox_nocredit_);

  y += 30;
  button_ok_ = button_create("Test", 120, y, 150, 28, id_button_ok_);

//  if (!form_)
// TODO: error handling
}


HWND
Dialog::button_create(const char* name, int x, int y, int w, int h, int &id)
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
Dialog::checkbox_create(const char* name,
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
		 WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL,
		 x + w1 + 25, y, w2, h, form_, (HMENU) ++ids_,
		 (HINSTANCE) GetWindowLong(form_, GWL_HINSTANCE), 0);
  id = ids_;

  return res;
}

void
Dialog::fill_resolutions_combobox()
{
  bool mineIncluded = false;
  int mine = 0;
  int inserted = 0;
  int width  = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);

  DetectResolutions();
  for (int i = 0; i < resolutions.size; ++i)
  {
    if (resolutions[i].w == width &&
	resolutions[i].h == height)
    {
      mineIncluded = true;
      mine = inserted;
    }
    inserted += 1;
    SendMessage(combobox_resolutions_, CB_ADDSTRING, 0, (LPARAM) resolutions[i].desc);
  }
  if (!mineIncluded)
  {
    inserted += 1;
    SendMessage(combobox_resolutions_, CB_ADDSTRING, 0, (LPARAM) "Current screen resolution");
  }

  SendMessage(combobox_resolutions_, CB_SETCURSEL, (WPARAM) mine, 0);
}

static int guessRatio(int width, int height)
{
  const float ratio = float(width) / float(height);
  int bestGuess = 0;
  float bestError = msys_fabsf(ratio - float(ratios[bestGuess].w) / float(ratios[bestGuess].h));
  for (int i = 0; i < ARRAY_LEN(ratios); ++i)
  {
    float error = msys_fabsf(ratio - float(ratios[i].w) / float(ratios[i].h));
    if (error < bestError)
    {
      bestGuess = i;
      bestError = error;
    }
  }
  return bestGuess;
}

void
Dialog::fill_ratios_combobox()
{
  int width  = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);

  const int numberOfRatios = sizeof(ratios) / sizeof(RatioDesc);
  for (int r = 0; r < numberOfRatios; ++r)
  {
    SendMessage(combobox_ratios_, CB_ADDSTRING, 0, (LPARAM) ratios[r].desc);
  }

  int guessedRatio = guessRatio(width, height);
  SendMessage(combobox_ratios_, CB_SETCURSEL, (WPARAM) guessedRatio, 0);
}


LRESULT CALLBACK
Dialog::input(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
  int wmi;
  int wme;

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
	instance_->credits_    = !instance_->is_checked(instance_->id_checkbox_nocredit_);

	// Résolution
	instance_->width_ = GetSystemMetrics(SM_CXSCREEN);
	instance_->height_ = GetSystemMetrics(SM_CYSCREEN);
	int current = 0;
	for (int i = 0; i < resolutions.size; ++i)
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
