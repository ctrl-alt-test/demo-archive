#ifndef STRING_UTILS_HH
#define STRING_UTILS_HH

#if DEBUG
#include <string>
#endif // DEBUG

namespace Core
{
#if DEBUG
	std::string&	RemoveComments(std::string& content);
	const char*		SkipSpaces(const char* input);
	std::string&	ReplaceAll(std::string& str, const char* pattern, const char* replace);
	std::string&	ReplaceIdentifier(std::string& str, const char* identifier, const char* replace);
#endif // DEBUG

	void reverse(char str[], int length);

	/// <summary>
	/// Writes num into str in base 10.
	/// </summary>
	/// <returns>The position _after_ the number.</returns>
	char* itoa(unsigned long num, char* str);
}

#endif // STRING_UTILS_HH
