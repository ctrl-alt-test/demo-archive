//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_H_
#define _MSYS_H_

#include "msys_types.h"
#include "msys_libc.h"
#include "msys_random.h"
#include "msys_malloc.h"
#include "msys_sound.h"
#include "msys_timer.h"
#include "msys_thread.h"
#include "msys_font.h"
#include "msys_debug.h"

#ifndef LINUX
# include "msys_glext.h"
#endif


const char * msys_init(uint64 h);
void msys_end();

void atexit(void* p);

#endif
