//
// dialog.hxx for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#ifndef DIALOG_HXX_
# define DIALOG_HXX_



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Includes

# include "dialog.hh"



//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Implementation

inline Dialog&
Dialog::instance()
{
  if (instance_ == NULL)
    instance_ = new Dialog();
  return *instance_;
}


inline bool
Dialog::test() const
{
  return test_;
}

inline bool
Dialog::fullscreen() const
{
  return fullscreen_;
}

inline bool
Dialog::sound() const
{
  return sound_;
}

inline int
Dialog::width() const
{
  return  width_;
}

inline int
Dialog::height() const
{
  return height_;
}


inline bool
Dialog::is_checked(int cb)
{
  return IsDlgButtonChecked(form_, cb) == BST_CHECKED;
}



#endif /* !DIALOG_HXX_ */
