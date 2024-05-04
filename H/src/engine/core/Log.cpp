#include "Log.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

//
// 1 to display log on debugger display,
// 0 for standard output.
//
// The debugger display appears in Visual Studio,
// or can also be captured by DebugView.
//
#define OUTPUT_TO_DEBUGGER_DISPLAY 1

using namespace Core;

static const char* levelHeaders[] = {
	nullptr, // No header for raw format
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"FATAL",
};

Log::Log()
{
	assert(sizeof(levelHeaders) / sizeof(levelHeaders[0]) == LogLevel::MaxLevel);
	for (int i = 0; i < LogLevel::MaxLevel; ++i)
	{
		count[i] = 0;
	}
}

void Log::Add(const LogLevel::Enum& level, const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	AddList(level, format, argList);
	va_end(argList);
}

void Log::AddList(const LogLevel::Enum& level, const char* format, va_list argList)
{
	assert(level < LogLevel::MaxLevel);
	count[level]++;

#if defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY

	const int maxLength = 2048;
	char buffer[maxLength] = { 0 };
	char* p = buffer;

	if (levelHeaders[level] != nullptr)
	{
		p += sprintf(p, "%s: ", levelHeaders[level]);
		p += vsnprintf(p, maxLength - (p - buffer), format, argList);
		if (p - buffer == 0 || p[-1] != '\n')
		{
			// Add a new line if the string doesn't already include one
			strcpy(p, "\n");
		}
	}
	else
	{
		p += vsnprintf(p, maxLength, format, argList);
	}
	OutputDebugString(buffer);

#else // !defined(_WIN32) || !OUTPUT_TO_DEBUGGER_DISPLAY

	if (level != LogLevel::Raw)
	{
		printf("%s: ", levelHeaders[level]);
		vprintf(format, argList);
		int len = strlen(format);
		if (len == 0 || format[len - 1] != '\n')
		{
			printf("\n");
		}
	}
	else
	{
		vprintf(format, argList);
	}

#endif // !defined(_WIN32) || !OUTPUT_TO_DEBUGGER_DISPLAY
}
