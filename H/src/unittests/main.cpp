#if defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY
#include "TestReporterVisualStudio.hh"
#endif
#include "UnitTest++/UnitTest++.h"

#if 0 // Enable to check that unit tests are working properly.
SUITE(SanityCheck)
{
	TEST(ShouldAlwaysFail)
	{
		CHECK(false);
	}

	TEST(ShouldAlwaysSucceed)
	{
		CHECK(true);
	}
}
#endif

int main(int /*argc*/, char** /*argv*/)
{
#if defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY

	TestReporterVisualStudio reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);

#else

	return UnitTest::RunAllTests();

#endif
}
