#ifndef PLATFORM_HH
#define PLATFORM_HH

#ifdef _WIN32

#include "Win32Platform.hh"
namespace platform
{
	typedef Win32Platform Platform;
}

#elif LINUX

#include "XLibPlatform.hh"
namespace platform
{
	typedef XLibPlatform Platform;
}

#else // !(_WIN32 || LINUX)

NOT_IMPLEMENTED;

#endif // !(_WIN32 || LINUX)

#endif // PLATFORM_HH
