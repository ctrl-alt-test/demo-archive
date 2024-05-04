#include "Debug.hh"

#ifdef _WIN32
#include <windows.h>
#else // !_WIN32
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#endif // !_WIN32

#if _HAS_EXCEPTIONS
#include <stdexcept>
#endif // _HAS_EXCEPTIONS

using namespace Core;

#ifdef ENABLE_LOG
Log Core::log;
#endif // ENABLE_LOG


#if _HAS_EXCEPTIONS

void Core::TerminateOnFatalError(const char* text)
{
	throw new std::exception(text);
}

NotImplementedException::NotImplementedException():
	std::logic_error("Not implemented.")
{
}

#else // !_HAS_EXCEPTIONS

void Core::TerminateOnFatalError(const char* text)
{
#ifdef _WIN32
	MessageBox(NULL, text, "Fatal error", MB_OK|MB_ICONERROR);

	// Using ExitProcess() instead of exit() which requires libc.
	ExitProcess(1);
#else // !_WIN32
	write(2, text, strlen(text));
	write(2, "\n", 1);
	exit(1);
#endif // !_WIN32
}

#endif // !_HAS_EXCEPTIONS
