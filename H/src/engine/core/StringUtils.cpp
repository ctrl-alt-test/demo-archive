#include "StringUtils.hh"
#include "Debug.hh"

#if DEBUG
#include <algorithm>
#include <cstring>
#endif

#if DEBUG

std::string& Core::RemoveComments(std::string& content)
{
	size_t pos = 0;
	while (true)
	{
		size_t openBlockComment = content.find("/*", pos);
		size_t openInlineComment = content.find("//", pos);
		if (openBlockComment == std::string::npos &&
			openInlineComment == std::string::npos)
		{
			break;
		}

		size_t openComment = 0;
		size_t closeComment = std::string::npos;
		if (openBlockComment != std::string::npos &&
			openBlockComment < openInlineComment)
		{
			openComment = openBlockComment;
			closeComment = content.find("*/", openComment + 2) + 2;
		}
		else if (openInlineComment != std::string::npos &&
			openInlineComment < openBlockComment)
		{
			openComment = openInlineComment;
			size_t close1 = content.find("\n", openComment + 2);
			size_t close2 = content.find("\r\n", openComment + 2);
			closeComment = (close2 == std::string::npos || close1 < close2) ? close1 : close2;
		}

		if (closeComment == std::string::npos)
		{
			LOG_FATAL("End of file in a comment.");
		}
		content.erase(openComment, closeComment - openComment);
		pos = openComment;
	}

	return content;
}

const char* Core::SkipSpaces(const char* input)
{
	const char* old;
	do
	{
		old = input;

		int len = 0;
		sscanf(input, "%*[ \r\n\t]%n", &len);
		input += len;

		len = 0;
		sscanf(input, "//%*[^\r\n]%n", &len);
		input += len;
	}
	while (input > old);

	return input;
}

std::string& Core::ReplaceAll(std::string& str, const char* pattern, const char* replace)
{
	const size_t patternLength = strlen(pattern);
	const size_t replaceLength = strlen(replace);

	if (patternLength > 0)
	{
		size_t pos = 0;
		while ((pos = str.find(pattern, pos)) != std::string::npos)
		{
			str.replace(pos, patternLength, replace);
			pos += replaceLength;
		}
	}
	return str;
}

std::string& Core::ReplaceIdentifier(std::string& str, const char* identifier, const char* replace)
{
	const size_t identifierLength = strlen(identifier);
	const size_t replaceLength = strlen(replace);

	if (identifierLength > 0)
	{
		size_t pos = 0;
		while ((pos = str.find(identifier, pos)) != std::string::npos)
		{
			if ((pos == 0 ||
				(!isalnum(str[pos - 1]) &&
				str[pos - 1] != '.')) && // don't replace "x" in "v.x"
				!isalnum(str[pos + identifierLength])) // don't replace "texture2" in "texture2D"
			{
				str.replace(pos, identifierLength, replace);
				pos += replaceLength;
			}
			else
			{
				pos += identifierLength;
			}
		}
	}
	return str;
}

#endif // DEBUG

void Core::reverse(char str[], int length)
{
	int start = 0;
	int end = length -1;
	while (start < end)
	{
		char tmp = str[start];
		str[start] = str[end];
		str[end] = tmp;
		start++;
		end--;
	}
}

char* Core::itoa(unsigned long num, char* str)
{
	int i = 0;
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	while (num != 0)
	{
		char rem = (char)(num % 10);
		str[i++] = rem + '0';
		num = num / 10;
	}
	str[i] = '\0';
	reverse(str, i);
	return str + i;
}
