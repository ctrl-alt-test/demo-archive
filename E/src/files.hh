#ifndef FILES_HH_
# define FILES_HH_

namespace Files
{
  void onFileChange(char *file, void (*callBack)());
  void checkFileChanges();
}

#endif
