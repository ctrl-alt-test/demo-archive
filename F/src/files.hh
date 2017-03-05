#ifndef FILES_HH_
# define FILES_HH_

namespace Files
{
  void onFileChange(const char *file, void (*callBack)(const char *filename));
  void checkFileChanges();
}

#endif
