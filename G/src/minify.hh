#if DEBUG

#ifndef		MINIFY_HH
# define	MINIFY_HH

#include "array.hh"
#include "sys/msys_forbidden.h"

class Minifier
{
public:
  Minifier();
  ~Minifier();
  void addFile(char* src, const char* filename);

private:
  FILE* fp;
  Array<const char*> files;
};

#endif
#endif
