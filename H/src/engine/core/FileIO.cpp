#include "FileIO.hh"
#include "engine/core/Debug.hh"
#include "engine/core/StringUtils.hh"
#include <cassert>

#if DEBUG

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <fstream>
#ifdef _WIN32
#include <Shlobj.h>
#endif

using namespace Core;

std::string Core::GetFullPath(const std::string& relativePath)
{
#ifdef _WIN32
   char fullPath[_MAX_PATH];
   if (_fullpath(fullPath, relativePath.c_str(), _MAX_PATH) != NULL)
   {
	   return std::string(fullPath);
   }
   return std::string();
#else // !_WIN32
	char fullPath[PATH_MAX];
	realpath(relativePath.c_str(), fullPath);
	return std::string(fullPath);
#endif // !_WIN32
}

//
// Efficient file loading. Credits go to:
// http://insanecoding.blogspot.jp/2011/11/how-to-read-in-file-in-c.html
//
std::string Core::ReadFileContent(const char* fileName)
{
	std::ifstream in(fileName, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize((unsigned int)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}

	LOG_ERROR("Could not load %s, error %d.", fileName, errno);
	return std::string();
}

bool Core::CreateDirectoryPath(const char* directoryPath)
{
#ifdef _WIN32
	int createDirectory = SHCreateDirectoryEx(NULL, directoryPath, NULL);
	return (createDirectory == ERROR_SUCCESS) || 
		(createDirectory == ERROR_ALREADY_EXISTS);
#endif // _WIN32
	return false;
}

void Core::WriteContentToFile(const std::string& fileName, const std::string& content)
{
#ifdef _WIN32
	size_t head = fileName.find_last_of("\\");
	if (head != std::string::npos)
	{
		std::string directoryPath = fileName.substr(0, head);
		if (!Core::CreateDirectoryPath(directoryPath.c_str()))
		{
			LOG_ERROR("Could not create directory %s.", directoryPath.c_str());
			return;
		}
	}
#endif

	std::ofstream out(fileName, std::ios::binary);
	if (out)
	{
		out << content;
		out.close();
	}
}

//
// TODO: support local path
// TODO: define macros at call
//
std::string	Core::PreprocessFile(const char* fileName, std::vector<std::string>* loadedFiles, std::string* outName)
{
	loadedFiles->push_back(std::string(fileName));

	std::string content = ReadFileContent(fileName);
	std::string newline = (content.find("\r\n") == std::string::npos ? "\n" : "\r\n");
	assert(content.size() > 0);

	size_t pos = 0;
	while (true)
	{
		pos = content.find("#include", pos);
		if (pos == std::string::npos)
		{
			break;
		}

		const size_t begin = content.find("\"", pos + 8) + 1;
		const size_t end = content.find("\"", begin);
		std::string includedFile = content.substr(begin, end - begin);
		std::string includedContent =
			"// Include begin: " + includedFile + newline +
			"// --------------------------------------------------------------------" + newline +
			ReadFileContent(includedFile.c_str()) +
			"// --------------------------------------------------------------------" + newline +
			"// Include end: " + includedFile + newline;
		content.replace(pos, end - pos + 1, includedContent);

		loadedFiles->push_back(includedFile);
	}

	//
	// Write preprocessed file for debugging
	//
	*outName = fileName;
	size_t head = outName->find_last_of("/");
	head = (head == std::string::npos) ? 0 : head + 1;
	outName->insert(head, "preprocessed.");
	outName->insert(0, "build/");
	*outName = GetFullPath(*outName);

	WriteContentToFile(*outName, content);

	return content;
}

#endif // DEBUG
