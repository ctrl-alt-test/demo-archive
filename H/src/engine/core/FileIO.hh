#ifndef FILE_IO_HH
#define FILE_IO_HH

#if DEBUG

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Core
{
	typedef std::unordered_map<std::string, std::string> ShaderReplacements;

	std::string	ReadFileContent(const char* fileName);
	void		WriteContentToFile(const std::string& fileName, const std::string& content);

	std::string GetFullPath(const std::string& relativePath);
	bool		CreateDirectoryPath(const char* directoryPath);

	/// <summary>
	/// Poor man's preprocessor: only supports #include directive, absolute paths only.
	/// Also, we do some string replacements.
	/// </summary>
	std::string	PreprocessFile(const char* fileName, std::vector<std::string>* loadedFiles, std::string* outName);
}

#endif // DEBUG

#endif // FILE_IO_HH
