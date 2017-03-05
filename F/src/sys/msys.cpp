//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#include "msys.h"
#include "msys_glext.h"
#include "msys_malloc.h"
#include "msys_font.h"

int msys_init( uint64 h )
{
    if( !msys_glextInit() )
    {
      DBG("Could not load OpenGL extensions!");
      return 0;
    }
    //if( !msys_mallocInit() )
    //{
    //  DBG("Could not initialize malloc!");
    //  return 0;
    //}

    msys_fontInit( h );

    return 1;
}

void msys_end( void )
{
    //msys_mallocEnd();
}

void *msys_memdup(const void *src, size_t size)
{
  void * res = msys_mallocAlloc(size);
  msys_memcpy(res, src, size);
  return res;
}

// (LLB) Le destructeur d'un objet global appelle implicitement atexit.
// On s'en fiche.
void atexit(void* p) {}
