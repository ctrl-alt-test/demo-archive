#ifdef DEBUG

#include <time.h>
#include <io.h>
#include <windows.h>

#include "array.hh"
#include "sys/msys.h"
#include "sys/msys_debug.h"

namespace Files
{

  struct FileData
  {
    const char* fileName;
    __time64_t lastChangedDate;
    void (*callBack)(const char *filename);
  };

  Array<FileData> filesToCheck(200);

  // Quand init est vrai (la première fois), on n'appelle pas de callback
  static void checkIfFileHasChanged(FileData& file, bool init)
  {
    _finddata_t fdata;
    long hfile = _findfirst(file.fileName, &fdata);
    if (hfile != -1)
    {
      if (fdata.time_write > file.lastChangedDate)
      {
	  file.lastChangedDate = fdata.time_write;
	  if (!init)
	  {
	    DBG("File '%s' has changed, reloading...", file.fileName);
            file.callBack(file.fileName);
	  }
      }
      _findclose(hfile);
    }
  }

  void onFileChange(const char *file, void (*callBack)(const char*))
  {
    FileData f = {_strdup(file), 0, callBack};

    _finddata_t fdata;
    if (_findfirst(file, &fdata) < 0)
    {
      DBG("Cannot monitor file '%s' (does not exist)", file);
      return;
    }

    // Evite les doublons
    for (int i = 0; i < filesToCheck.size; i++)
    {
      if (strcmp(file, filesToCheck[i].fileName) == 0)
        return;
    }

    filesToCheck.add(f);
    DBG("Monitoring '%s'", file);
  }

  void checkFileChanges()
  {
    static bool first = true;
    for (int i = 0; i < filesToCheck.size; i++)
      checkIfFileHasChanged(filesToCheck[i], first);
    first = false;
  }
}

#endif
