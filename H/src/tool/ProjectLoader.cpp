#include "ProjectLoader.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/core/Debug.hh"
#include "engine/core/FileIO.hh"
#include "engine/core/StringUtils.hh"
#include "engine/container/Dict.hxx"
#include "engine/noise/Hash.hh"
#include "engine/runtime/FileWatcher.hh"
#include "engine/timeline/Variable.hh"
#include <cstdio>
#include <cassert>

using namespace Tool;

#define MAX_VARIABLES	256 // This is just for the container allocation; change as needed

#if VAR_EDIT
Container::Dict<const char*, Timeline::Variable> Tool::allVariables(MAX_VARIABLES);

static
void exportVariablesList(const char* fileName, const char* macroName)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', variables list won't be exported.", fileName);
	}
	else
	{
		const Container::Array<const char*>& names = allVariables.keys;

		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "enum variableId {\n");
		for (int i = 0; i < names.size; ++i)
		{
			fprintf(fp, "    %s%s,\n", names[i], (i == 0 ? " = 0" : ""));
		}
		fprintf(fp, "\n    numberOfVariables,\n");
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}

static
void exportVariablesData(const char* fileName, const char* macroName)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', variables data won't be exported.", fileName);
	}
	else
	{
		const Container::Array<Timeline::Variable>& vars = allVariables.values;

		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "const int variableDef[] = {\n");
		for (int i = 0; i < vars.size; ++i)
		{
			const Timeline::Variable& var = vars[i];
			fprintf(fp, "    %d, %d, %d,\n", var.dim(), var.size(), (int)var.interpolation());
		}
		fprintf(fp, "};\n\n\n");

		fprintf(fp, "const float variableData[] = {\n");
		for (int i = 0; i < vars.size; ++i)
		{
			const Timeline::Variable& var = vars[i];
			for (int j = 0; j < var.size() * var.dim(); ++j)
			{
				fprintf(fp, " %.10ff,", Algebra::roundb(var.data()[j], 16));
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}
#endif // VAR_EDIT

ProjectLoader::ProjectLoader(runtime::FileWatcher* fileWatcher)
{
#if VAR_EDIT
	m_fileWatcher = fileWatcher;
#endif // VAR_EDIT
}

ProjectLoader::~ProjectLoader()
{
#if VAR_EDIT
	exportVariablesList("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_variableId.hh), "EXPORTED_VARIABLE_ID_HH");
	exportVariablesData("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_variablesData.hh), "EXPORTED_VARIABLES_DATA_HH");
#endif // VAR_EDIT
}

#if VAR_EDIT

void ProjectLoader::LoadAndMonitorProject(const char* fileName)
{
	std::string content = Core::ReadFileContent(fileName);
	readInput(content.c_str());

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
	if (m_fileWatcher != NULL)
	{
		int hash = Noise::Hash::get32(fileName);
		m_fileWatcher->OnFileUpdated(fileName, std::bind(&ProjectLoader::ReloadAndMonitorProject, this, fileName, std::placeholders::_1), hash);
	}
#endif // ENABLE_AUTOMATIC_FILE_RELOAD
}

void ProjectLoader::ReloadAndMonitorProject(const char* fileName, const char*)
{
	LOG_INFO("Reloading %s", fileName);
	LoadAndMonitorProject(fileName);
}

static
const char* readArray(const char* input, Container::Array<float>& arr)
{
	int len;
	for (int i = 0; i < arr.max_size; ++i)
	{
		input = Core::SkipSpaces(input);
		float f;
		int ret = sscanf(input, "%f%n", &f, &len);
		if (ret != 1)
		{
			LOG_ERROR("Could not parse from:");
			LOG_RAW(input);
		}
		assert(ret == 1);
		arr.add(f);
		input += len;
		// optional comma
		len = 0;
		sscanf(input, ",%n", &len);
		input += len;
	}
	input = Core::SkipSpaces(input);
	len = 0;
	sscanf(input, "; %n", &len);
	assert(len > 0);
	input += len;
	return input;
}

void ProjectLoader::readInput(const char* input)
{
	while (*input)
	{
		char name[128];
		int dim, size, len;
		input = Core::SkipSpaces(input);
		int ret = sscanf(input, "%80[^ \n] %n", name, &len);
		if (ret < 1)
		{
			break;
		}
		input += len;
		if (strcmp(name, "command") == 0)
		{
			int command;
			char args[200] = {0};
			ret = sscanf(input, "%d %s%n", &command, args, &len);
			if (ret < 1)
			{
				break;
			}
			//
			// FIXME: executeCommand(command, args);
			//
		}
		else
		{
			int interpolation;
			ret = sscanf(input, "%d %d %d%n", &size, &dim, &interpolation, &len);
			if (ret < 2)
			{
				break;
			}
			input += len;

			Container::Array<float> data(size * dim);
			input = readArray(input, data);

			Timeline::Variable* v = allVariables[name];
			if (v == NULL)
			{
				LOG_INFO("New variable '%s', dim = %d, size = %d", name, dim, size);
				char* nameCopy = strdup(name);
				allVariables.add(nameCopy, Timeline::Variable(data.elt, dim, size, (Timeline::InterpolationMode)interpolation));
			}
			else
			{
				LOG_INFO("Updating '%s', dim = %d, size = %d", name, dim, size);
				v->update(data.elt, dim, size, (Timeline::InterpolationMode)interpolation);
			}
		}
	}
	if (*input != 0)
	{
		LOG_WARNING("unparsed: <<%s>>", input);
	}
}

#else // !VAR_EDIT

#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_variablesData.hh)

Timeline::Variable Tool::allVariables[numberOfVariables];

void ProjectLoader::LoadProject()
{
	int dataOffset = 0;
	size_t defIndex = 0;
	for (size_t i = 0; i < numberOfVariables; ++i)
	{
		const float* data = variableData + dataOffset;
		const int dim = variableDef[defIndex++];
		const int size = variableDef[defIndex++];
		const int interpolation = variableDef[defIndex++];

		allVariables[i] = Timeline::Variable(data, dim, size, (Timeline::InterpolationMode)interpolation);
		dataOffset += dim * size;
	}
}

#endif // !VAR_EDIT
