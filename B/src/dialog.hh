//
// dialog.hh for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#ifndef DIALOG_HH_
# define DIALOG_HH_



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

# include <windows.h>



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Declarations

class Dialog
{
  public:
    static Dialog& instance();

    void run();

    bool test() const;
    bool fullscreen() const;
    bool sound() const;
    bool gamma() const;
    int  width() const;
    int  height() const;

  private:
    Dialog();
    Dialog(const Dialog&);
    Dialog& operator = (const Dialog&);

    void form_register();
    void form_init();

    HWND button_create  (const char* text,
			 int x, int y, int w, int h, int& id);
    HWND checkbox_create(const char* text,
			 int x, int y, int w, int h,
			 bool checked, int &id);

    void combobox_create (int x, int y, int w, int h);

    bool is_checked(int cb);

    static LRESULT CALLBACK input(HWND h, UINT msg, WPARAM wp, LPARAM lp);

    static Dialog * instance_;

    HINSTANCE hinstance_;
    HWND form_;
    HWND button_ok_;
    HWND button_ko_;
    HWND checkbox_fs_;
    HWND checkbox_sound_;
    HWND checkbox_gamma_;
    HWND combobox_resolutions_;
    int id_button_ok_;
    int id_button_ko_;
    int id_checkbox_fs_;
    int id_checkbox_sound_;
    int id_checkbox_gamma_;
    int id_combobox_resolutions_;

    bool test_;
    bool fullscreen_;
    bool gamma_;
    bool sound_;
    int  width_;
    int  height_;

    int ids_;
};



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

# include "dialog.hxx"



#endif /* !DIALOG_HH_ */
