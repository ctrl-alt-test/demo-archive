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
    bool  fullscreen() const;
//     bool gamma() const;
    bool  credits() const;
    int   width() const;
    int   height() const;
    float ratio() const;

  private:
    Dialog();
    Dialog(const Dialog&);
    Dialog& operator = (const Dialog&);

    void form_register();
    void form_init();

    HWND button_create  (const char* name,
			 int x, int y, int w, int h, int& id);
    HWND checkbox_create(const char* name,
			 int x, int y, int w, int h,
			 bool checked, int &id);
    HWND combobox_create(const char* name,
			 int x, int y, int w1, int w2, int h, int &id);

    void fill_resolutions_combobox();
    void fill_ratios_combobox();

    bool is_checked(int cb);

    static LRESULT CALLBACK input(HWND h, UINT msg, WPARAM wp, LPARAM lp);

    static Dialog * instance_;

    HINSTANCE hinstance_;
    HWND form_;
    HWND button_ok_;
    HWND checkbox_fs_;
//     HWND checkbox_gamma_;
    HWND checkbox_nocredit_;

    HWND combobox_resolutions_;
    HWND combobox_ratios_;
    int id_button_ok_;
    int id_checkbox_fs_;
//     int id_checkbox_gamma_;
    int id_checkbox_nocredit_;
    int id_combobox_resolutions_;
    int id_combobox_ratios_;

    bool test_;
    bool fullscreen_;
//     bool gamma_;
    bool credits_;
    int  width_;
    int  height_;
    float ratio_;

    int ids_;
};

#endif /* !DIALOG_HH_ */
