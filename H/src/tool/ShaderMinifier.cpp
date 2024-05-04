#include "ShaderMinifier.hh"

#include "IncludeUtils.hh"
#include "ShaderUniformNames.hh"
#include "engine/core/Debug.hh"
#include "engine/core/FileIO.hh"
#include "engine/core/StringUtils.hh"
#include <cstring>

// 1 for a quick and dirty renaming before minifying the shaders.
#define MINIFY_UNIFORM_IDENTIFIERS 1

// 0 to call ShaderMinifier; 1 for quick ad hoc minification.
#define FAST_MINIFICATION 0

using namespace Tool;

#if FAST_MINIFICATION
static bool search(char c, const char* str)
{
	for (const char *ptr = str; *ptr; ++ptr)
	{
		if (*ptr == c)
		{
			return true;
		}
	}
	return false;
}
#endif //FAST_MINIFICATION

Minifier::Minifier()
{
}

Minifier::~Minifier()
{
}

void Minifier::MinifyAllFiles() const
{
#if MINIFY_UNIFORM_IDENTIFIERS
	// Replace the uniform, vertex attribute and varying identifiers.
	// Ideally it should be done by the ShaderMinifier itself, but this
	// is a workaround until it's implemented.
	Core::ShaderReplacements identifierReplacements = getShaderReplacements();
	for (size_t i = 0; i < files.size(); ++i)
	{
		const char* fileName = files[i].c_str();
		std::string content = Core::ReadFileContent(fileName);

		for (auto it = identifierReplacements.begin(); it != identifierReplacements.end(); it++)
		{
			const char* id = it->second.c_str();
			const char* minifiedId = it->first.c_str();
			Core::ReplaceIdentifier(content, id, minifiedId);
		}
		Core::WriteContentToFile(fileName, content);
	}
#endif // MINIFY_UNIFORM_IDENTIFIERS

#if FAST_MINIFICATION
	const char *fileName = "src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaders.cpp);
	FILE* fp = fopen(fileName, "wb");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', shaders won't be minified.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n\n", __FUNCTION__, __FILE__, __LINE__);
		for (size_t i = 0; i < files.size(); ++i)
		{
			FastMinify(fp, files[i].c_str());
		}
		fclose(fp);
	}
#else // !FAST_MINIFICATION
	ExternalMinify();
#endif // !FAST_MINIFICATION
}

int Minifier::AddFile(const std::string& filename)
{
	for (int i = 0; i < (int)files.size(); ++i)
	{
		if (files[i] == filename)
		{
			return i;
		}
	}
	files.push_back(filename);
	return (int)files.size() - 1;
}

#if FAST_MINIFICATION

void Minifier::FastMinify(FILE* fp, const char* filename) const
{
	if (fp == NULL)
	{
		return;
	}

	std::string content = Core::ReadFileContent(filename);
	const char* data = content.c_str();

	fprintf(fp, "// %s\n", filename);
	bool comment = false;	// End of line comments: //...
	bool commentm = false;	// Multiple lines comments: /*...*/
	bool squeeze = true;	// Remove whitespaces.
	bool macro = false;
	char* result = strdup(data);
	char* ptr2 = result;
	for (const char * ptr = data; *ptr; ptr++)
	{
		if (commentm)
		{
			if (*ptr == '/' && ptr[-1] == '*')
			{
				commentm = false;
			}
			continue;
		}
		if (macro)
		{
			if (*ptr == '\n')
			{
				*ptr2++ = '\\';
				*ptr2++ = 'n';
				macro = false;
				squeeze = true;
			}
			else
			{
				*ptr2++ = *ptr;
			}
			continue;
		}
		if (comment)
		{
			if (*ptr == '\n') comment = false;
			continue;
		}

		if (*ptr == '/' && ptr[1] == '/')
		{
			comment = true;
			continue;
		}
		if (*ptr == '/' && ptr[1] == '*')
		{
			commentm = true;
			continue;
		}
		if (*ptr == '#')
		{
			// '#' must be at the beginning of the line.
			if (ptr2 > result + 1 && ptr2[-2] != '\\' && ptr2[-1] != 'n')
			{
				*ptr2++ = '\\';
				*ptr2++ = 'n';
			}
			macro = true;
		}
		if (squeeze && search(*ptr, " \t\r\n"))
		{
			continue;
		}
		squeeze = search(*ptr, "+-*/=;(){}[],|&<> \t\r\n");

		if (squeeze && ptr2[-1] == ' ' && search(*ptr, "+-*/=;(){},|&<>"))
		{
			ptr2--;
		}

		// No new line in a string.
		if (*ptr == '\r' || *ptr == '\n')
		{
			*ptr2++ = ' ';
		}
		else
		{
			*ptr2++ = *ptr;
		}
	}
	*ptr2 = '\0';
	fprintf(fp, "\"%s\",\n", result);
	LOG_INFO("Fast-minified %s: %d -> %d bytes.", filename, content.size(), strlen(result));
	free(result);
}

#else // !FAST_MINIFICATION

void Minifier::ExternalMinify() const
{
	const char batchFile[] = "minifyShaders.bat";
	char cmd[5000] = {0};

	// Generate command line
	LOG_INFO("Creating shader minifier command...");
	strcat(cmd, "..\\tools\\shader_minifier.exe ");
	strcat(cmd, "-o " "src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaders.cpp) " --format c-array -v --preserve-externals ");
	for (size_t i = 0; i < files.size(); ++i)
	{
		strcat(cmd, " ");
		strcat(cmd, files[i].c_str());
	}
	strcat(cmd, " || pause");
	LOG_INFO("cmd = %s", cmd);

	// Write down .bat script
	FILE* out = fopen(batchFile, "wb");
	if (out != NULL)
	{
		fprintf(out, "%s", cmd);
		fclose(out);

		// Run command
		LOG_INFO("Invoking shader minifier...");
		system(batchFile);
	}
	else
	{
		LOG_ERROR("Cannot create script '%s', shaders won't be minified.", batchFile);
	}
}

#endif // !FAST_MINIFICATION
