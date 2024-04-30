//
// dialog.hh for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#ifndef WIN32_DIALOG_HH
#define WIN32_DIALOG_HH

#include "engine/container/Array.hh"
#include "engine/core/Settings.hh"
#include <windows.h>

namespace platform
{
	class Dialog
	{
	public:
		Dialog();

		void run(const char* title);

	public:
		bool			isOk;
		bool			fullscreen;
		int				renderWidth;
		int				renderHeight;
		int				monitorLeft;
		int				monitorTop;
		int				monitorWidth;
		int				monitorHeight;
		int				dpi;
		float			displayAspectRatio;

		static Dialog*	s_instance;

	private:
		Dialog(const Dialog&);
		Dialog& operator = (const Dialog&);

		HWND button_create  (const char* name,
							 int x, int y,
							 int w, int h,
							 int* id);
		HWND checkbox_create(const char* name,
							 int x, int y,
							 int w, int h,
							 bool isChecked,
							 int* id);
		HWND combobox_create(const char* name,
							 int x, int y,
							 int w1, int w2,
							 int h,
							 int* id);

		void fill_resolutions_combobox();
		void fill_ratios_combobox();

		static LRESULT CALLBACK input(HWND h, UINT msg, WPARAM wp, LPARAM lp);

		HINSTANCE	m_hInstance;
		HWND		m_hDlg;
		HFONT		m_hFont;

		HWND		m_combobox_resolutions;
		HWND		m_combobox_ratios;
		HWND		m_checkbox_fs;
		HWND		m_button_ok;

		int			m_id_combobox_resolutions;
		int			m_id_combobox_ratios;
		int			m_id_checkbox_fs;
		int			m_id_button_ok;

		int			m_ids;
	
#if ENABLE_QUALITY_OPTION
	public:
		int			quality;
	private:
		void		fill_qualities_combobox();
		HWND		m_combobox_qualities;
		int			m_id_combobox_qualities;
#endif // ENABLE_QUALITY_OPTION
	};
}

#endif // WIN32_DIALOG_HH
