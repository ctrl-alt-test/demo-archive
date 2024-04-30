#include "engine/core/StringUtils.hh"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

SUITE(StringUtils)
{
	TEST(ReplaceAll)
	{
		{
			std::string testString;
			Core::ReplaceAll(testString, "", "");
			CHECK(testString.empty());
		}
		{
			std::string testString = "toto";
			Core::ReplaceAll(testString, "o", "a");
			CHECK_EQUAL("tata", testString);
		}
		{
			std::string testString = "toto";
			Core::ReplaceAll(testString, "a", "i");
			CHECK_EQUAL("toto", testString);
		}
		{
			std::string testString = "totootoootoooo";
			Core::ReplaceAll(testString, "oo", "a");
			CHECK_EQUAL("totataotaa", testString);
		}
	}

	TEST(ReplaceIdentifier)
	{
		{
			std::string testString;
			Core::ReplaceIdentifier(testString, "", "");
			CHECK(testString.empty());
		}
		{
			std::string testString = "x = x0 + 2*x;";
			Core::ReplaceIdentifier(testString, "x", "y");
			CHECK_EQUAL("y = x0 + 2*y;", testString);
		}
		{
			std::string testString = "color = texture2D(tex, uv).rgb;";
			Core::ReplaceIdentifier(testString, "tex", "t");
			CHECK_EQUAL("color = texture2D(t, uv).rgb;", testString);
		}
		{
			std::string testString = "x = input.x;";
			Core::ReplaceIdentifier(testString, "x", "y");
			CHECK_EQUAL("y = input.x;", testString);
		}
	}
}
