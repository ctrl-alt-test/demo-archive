// crt0tcon.cpp

// based on:
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001

// 08/12/06 (mv)

#include <windows.h>
#include "libct.h"
#include <stdio.h>

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcmt.lib")
#pragma comment(linker, "/nodefaultlib:libcmtd.lib")

EXTERN_C int _tmain(int, TCHAR **, TCHAR **);    // In user's code

#ifdef UNICODE
EXTERN_C void wmainCRTStartup()
#else
EXTERN_C void mainCRTStartup()
#endif
{
#ifdef ENABLE_COMMAND_LINE
    int argc = _init_args();
#else // !ENABLE_COMMAND_LINE
	int argc = 0;
	TCHAR **_argv = NULL;
#endif // !ENABLE_COMMAND_LINE
    _init_atexit();
#ifdef ENABLE_FILE_IO
	_init_file();
#endif // ENABLE_FILE_IO
    _initterm(__xc_a, __xc_z);

    int ret = _tmain(argc, _argv, 0);

	_doexit();
#ifdef ENABLE_COMMAND_LINE
	_term_args();
#endif // ENABLE_COMMAND_LINE
    ExitProcess(ret);
}