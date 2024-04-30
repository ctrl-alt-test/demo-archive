//
// dialog.hxx for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#ifndef		DIALOG_HXX
# define	DIALOG_HXX

#include "dialog.hh"

inline Dialog& Dialog::instance()
{
  if (instance_ == NULL)
    instance_ = new Dialog();
  return *instance_;
}

inline bool Dialog::test() const { return test_; }

inline bool Dialog::fullscreen() const { return fullscreen_; }

inline bool Dialog::credits() const { return credits_; }

inline int Dialog::width() const { return  width_; }

inline int Dialog::height() const { return height_; }

inline float Dialog::ratio() const { return ratio_; }

inline bool Dialog::is_checked(int cb)
{
  return IsDlgButtonChecked(form_, cb) == BST_CHECKED;
}

#endif		// DIALOG_HXX
