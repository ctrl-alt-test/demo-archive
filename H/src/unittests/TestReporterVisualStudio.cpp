#include "TestReporterVisualStudio.hh"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

#if defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY
#include <windows.h>

void TestReporterVisualStudio::PrintVS(const char* format, ...)
{
	char buffer[4096] = { 0 };
	const int maxLength = sizeof(buffer)/sizeof(buffer[0]) - 1;
	va_list argList;
	va_start(argList, format);
	vsnprintf(buffer, maxLength, format, argList);
	vfprintf(stderr, format, argList);
	va_end(argList);
	OutputDebugString(buffer);
}

void TestReporterVisualStudio::ReportTestStart(UnitTest::TestDetails const& test)
{
}

void TestReporterVisualStudio::ReportFailure(UnitTest::TestDetails const& test, char const* failure)
{
	using namespace std;
	char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";
	PrintVS(errorFormat, test.filename, test.lineNumber, test.testName, failure);
}

void TestReporterVisualStudio::ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed)
{
}

void TestReporterVisualStudio::ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
{
	using namespace std;

	char buffer[4096] = { 0 };

	if (failureCount > 0)
	{
		PrintVS("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
	}
	else
	{
		PrintVS("Success: %d tests passed.\n", totalTestCount);
	}
	PrintVS("Test time: %.2f seconds.\n", secondsElapsed);
}

#endif // defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY
