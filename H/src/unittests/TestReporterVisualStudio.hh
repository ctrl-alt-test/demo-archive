#ifndef TEST_REPORTER_VISUAL_STUDIO_HH
#define TEST_REPORTER_VISUAL_STUDIO_HH

#include "UnitTest++/TestReporter.h"

class UNITTEST_LINKAGE TestReporterVisualStudio : public UnitTest::TestReporter
{
private:
	static void PrintVS(const char* format, ...);

    virtual void ReportTestStart(UnitTest::TestDetails const& test);
    virtual void ReportFailure(UnitTest::TestDetails const& test, char const* failure);
    virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed);
    virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
};

#endif // TEST_REPORTER_VISUAL_STUDIO_HH
