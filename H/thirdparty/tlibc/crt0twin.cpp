// crt0twin.cpp

// based on:
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001

// 08/12/06 (mv)

#include <windows.h>
#include "libct.h"

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcmt.lib")
#pragma comment(linker, "/nodefaultlib:libcmtd.lib")

#ifdef ENABLE_COMMAND_LINE
int __argc;
TCHAR **__targv;
#endif ENABLE_COMMAND_LINE

#ifdef UNICODE
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int);
#define _tWinMainCRTStartup wWinMainCRTStartup
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
#define _tWinMainCRTStartup WinMainCRTStartup
#endif

EXTERN_C void _tWinMainCRTStartup()
{
#ifdef ENABLE_COMMAND_LINE
	__argc = _init_args();
	__targv = _argv;
#endif ENABLE_COMMAND_LINE

#ifdef ENABLE_FILE_IO
	_init_file();
#endif // ENABLE_FILE_IO

#ifdef ENABLE_COMMAND_LINE
	TCHAR *cmd = GetCommandLine();

	// Skip program name
	if (*cmd == _T('"'))
	{
	    while (*cmd && *cmd != _T('"'))
	        cmd++;
	    if (*cmd == _T('"'))
	        cmd++;
	}
	else
	{
	    while (*cmd > _T(' '))
	        cmd++;
	}

	// Skip any white space
	while (*cmd && *cmd <= _T(' '))
	    cmd++;
#else // !ENABLE_COMMAND_LINE
	TCHAR *cmd = "";
#endif // !ENABLE_COMMAND_LINE

	STARTUPINFO si;
	si.dwFlags = 0;
	GetStartupInfo(&si);

	_init_atexit();
	_initterm(__xc_a, __xc_z);			// call C++ constructors

	int ret = _tWinMain(GetModuleHandle(0), 0, cmd, si.dwFlags&STARTF_USESHOWWINDOW ? si.wShowWindow : SW_SHOWDEFAULT);

	_doexit();
#if ENABLE_COMMAND_LINE
	_term_args();
#endif // ENABLE_COMMAND_LINE
	ExitProcess(ret);
}