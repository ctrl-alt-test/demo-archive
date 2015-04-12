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
    char* fileName;
    __time64_t lastChangedDate;
    void (*callBack)();
  };

  Array<FileData> filesToCheck(200);

  void checkIfFileHasChanged(FileData& file)
  {
    _finddata_t fdata;
    long hfile = _findfirst(file.fileName, &fdata);
    if (hfile != -1)
    {
      if (fdata.time_write != file.lastChangedDate)
      {
	  file.lastChangedDate = fdata.time_write;
	  file.callBack();
	  DBG("File '%s' has changed, reloading...", file.fileName);
      }
      _findclose(hfile);
    }
  }

  void onFileChange(char *file, void (*callBack)())
  {
    FileData f = {file, 0, callBack};
    filesToCheck.add(f);
    DBG("Monitoring '%s'", file);
  }

  void checkFileChanges()
  {
    static long lastCheck = 0;
    long now = msys_timerGet();
    if (now - lastCheck < 200) // toutes les 200ms
      return;

    lastCheck = now;
    for (int i = 0; i < filesToCheck.size; i++)
      checkIfFileHasChanged(filesToCheck[i]);
  }
}

#endif
