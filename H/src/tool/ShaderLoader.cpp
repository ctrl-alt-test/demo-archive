#include "ShaderLoader.hh"

#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/FileIO.hh"
#include "engine/noise/Hash.hh"
#include "engine/runtime/FileWatcher.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#if SHADER_EDIT
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#endif

using namespace Gfx;
using namespace Tool;

namespace LoadingBar { void update(void* = nullptr); }

#if SHADER_EDIT
void ExportShaderNames(const char* fileName, const char* macroName,
					   const std::vector<ShaderInfo>& loadedShaders,
					   const std::vector<std::pair<ShaderInfo, std::string> >& shaderAliases)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', shaders ids won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "enum ShaderName {\n");
		for (size_t i = 0; i < loadedShaders.size(); ++i)
		{
			const ShaderInfo& info = loadedShaders[i];
			fprintf(fp, "	// %s,\n", info.vsFileName.c_str());
			fprintf(fp, "	// %s,\n", info.fsFileName.c_str());
			fprintf(fp, "	%s,\n", info.name.c_str());

			for (size_t j = 0; j < shaderAliases.size(); ++j)
			{
				if (info.name == shaderAliases[j].second)
				{
					fprintf(fp, "	%s = %s,\n", shaderAliases[j].first.name.c_str(), shaderAliases[j].second.c_str());
				}
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "	NumberOfShaders,\n");
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}

void ExportShaderDependencies(const char* fileName, const char* macroName,
							  const std::vector<ShaderInfo>& loadedShaders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', shaders ids won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "const ShaderDependency shaderDependencies[] = {\n");
		fprintf(fp, "	// name                           vs     fs\n");
		for (size_t i = 0; i < loadedShaders.size(); ++i)
		{
			const ShaderInfo& info = loadedShaders[i];
			fprintf(fp, "	{ %-30s, %-5d, %-5d },\n", info.name.c_str(), info.vsIndex, info.fsIndex);
		}
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}
#endif // SHADER_EDIT

ShaderLoader::ShaderLoader(IGraphicLayer* gfxLayer,
						   runtime::FileWatcher* fileWatcher,
						   bool minifyAndExportOnDestroy):
	m_gfxLayer(gfxLayer)
{
#if SHADER_EDIT
	m_fileWatcher = fileWatcher;
	m_minifyAndExportOnDestroy = minifyAndExportOnDestroy;
#endif // SHADER_EDIT
}

ShaderLoader::~ShaderLoader()
{
#if SHADER_EDIT
	if (m_minifyAndExportOnDestroy)
	{
		m_minifier.MinifyAllFiles();
		ExportShaderNames("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaderNames.hh), "EXPORTED_SHADER_IDS", m_loadedShaders, m_shaderAliases);
		ExportShaderDependencies("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaderDependencies.hh), "EXPORTED_SHADER_DEPENDENCIES", m_loadedShaders);
	}
#endif // SHADER_EDIT
}

#if SHADER_EDIT

ShaderID ShaderLoader::CreateLoadAndMonitorShader(const char* name, const char* vsFileName, const char* fsFileName, const char* callLocation)
{
	ShaderInfo info;
	info.hash = Noise::Hash::get32(vsFileName) + Noise::Hash::get32(fsFileName);
	info.name = name;
	info.id = ShaderID::InvalidID;
	info.vsIndex = -1;
	info.fsIndex = -1;
	info.vsFileName = vsFileName;
	info.fsFileName = fsFileName;
	info.loadLocation = callLocation;

	bool shouldLoadShader = true;

	for (size_t i = 0; i < m_shaderAliases.size(); ++i)
	{
		const ShaderInfo& test = m_shaderAliases[i].first;
		if (test.name == info.name)
		{
			shouldLoadShader = false;
			if (test.hash != info.hash)
			{
				LOG_FATAL("More than one shader is named %s:\n%s + %s\n%s + %s",
					info.name.c_str(),
					test.vsFileName.c_str(), test.fsFileName.c_str(),
					info.vsFileName.c_str(), info.fsFileName.c_str());
				LOG_RAW("%s: See first declaration of %s.\n", test.loadLocation.c_str(), test.name.c_str());
				LOG_RAW("%s: See conflicting declaration of %s.\n", info.loadLocation.c_str(), info.name.c_str());
			}
			break;
		}
	}

	for (size_t i = 0; i < m_loadedShaders.size(); ++i)
	{
		const ShaderInfo& test = m_loadedShaders[i];
		if (test.hash == info.hash)
		{
			if (test.name != info.name)
			{
				// Do not add an alias if there was already one.
				if (shouldLoadShader)
				{
					m_shaderAliases.push_back(std::pair<ShaderInfo, std::string>(info, test.name));
				}
			}

			return test.id;
		}
		else if (test.name == info.name)
		{
			shouldLoadShader = false;
			LOG_FATAL("More than one shader is named %s:\n%s + %s\n%s + %s",
				info.name.c_str(),
				test.vsFileName.c_str(), test.fsFileName.c_str(),
				info.vsFileName.c_str(), info.fsFileName.c_str());
			LOG_RAW("%s: See declaration of %s.\n", test.loadLocation.c_str(), test.name.c_str());
			LOG_RAW("%s: See declaration of %s.\n", info.loadLocation.c_str(), info.name.c_str());
			break;
		}
	}

	if (shouldLoadShader)
	{
		info.id = m_gfxLayer->CreateShader();
		m_loadedShaders.push_back(info);
		LoadAndMonitorShader(m_loadedShaders.size() - 1, vsFileName, fsFileName);
		LoadingBar::update();
		return info.id;
	}

	return ShaderID::InvalidID;
}

void ShaderLoader::LoadAndMonitorShader(size_t index, const char* vsFileName, const char* fsFileName)
{
	ShaderInfo& info = m_loadedShaders[index];

	std::vector<std::string> loadedFiles;
	std::string preprocessedVsFileName;
	std::string preprocessedFsFileName;
	const std::string vsContent = Core::PreprocessFile(vsFileName, &loadedFiles, &preprocessedVsFileName);
	const std::string fsContent = Core::PreprocessFile(fsFileName, &loadedFiles, &preprocessedFsFileName);
	LOG_INFO("Loading shaders %s and %s...", preprocessedVsFileName.c_str(), preprocessedFsFileName.c_str());

#if _HAS_EXCEPTIONS && ENABLE_AUTOMATIC_FILE_RELOAD
	// If exceptions are enabled, shader compilation is going to throw
	// when it fails. But if file reloading is enabled, we want to
	// catch that exception and only show an error, so we can fix the
	// problem without interrupting the program.
	try
	{
		m_gfxLayer->LoadShader(info.id, vsContent.c_str(), preprocessedVsFileName.c_str(), fsContent.c_str(), preprocessedFsFileName.c_str());
	}
	catch (...)
	{
		LOG_FATAL("Shader compilation error; shader '%s' and '%s' not loaded.", preprocessedVsFileName.c_str(), preprocessedFsFileName.c_str());
	}
#else // !_HAS_EXCEPTIONS || !ENABLE_AUTOMATIC_FILE_RELOAD
	m_gfxLayer->LoadShader(info.id, vsContent.c_str(), preprocessedVsFileName.c_str(), fsContent.c_str(), preprocessedFsFileName.c_str());
#endif // !_HAS_EXCEPTIONS || !ENABLE_AUTOMATIC_FILE_RELOAD

	info.vsIndex = m_minifier.AddFile(preprocessedVsFileName);
	info.fsIndex = m_minifier.AddFile(preprocessedFsFileName);

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
	if (m_fileWatcher != NULL)
	{
		for (size_t i = 0; i < loadedFiles.size(); ++i)
		{
			const std::string& file = loadedFiles[i];
			unsigned int callbackHash = Noise::Hash::get32(file.c_str()) + Noise::Hash::get32(index);
			m_fileWatcher->OnFileUpdated(file.c_str(),
				std::bind(&ShaderLoader::ReloadAndMonitorShader, this, index, vsFileName, fsFileName, std::placeholders::_1),
				callbackHash);
		}
	}
#endif // ENABLE_AUTOMATIC_FILE_RELOAD
}

void ShaderLoader::ReloadAndMonitorShader(size_t index, const char* vsFileName, const char* fsFileName, const char*)
{
	LOG_INFO("Reloading %s and %s", vsFileName, fsFileName);
	LoadAndMonitorShader(index, vsFileName, fsFileName);
}

#else // !SHADER_EDIT

struct ShaderDependency
{
	ShaderName	name;
	int			vs;
	int			fs;
};

#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaderDependencies.hh)

const char* shaderSources[] = {
#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaders.cpp)
};

// FIXME: when SHADER_EDIT is not defined, this function must be called
// before using the shaders. Otherwise the program will crash on the
// first drawcall with little explanation of why. Ideally, we'd want
// LOAD_SHADER to report the error explicitely.
void ShaderLoader::LoadShaders()
{
	for (int i = 0; LoadShadersIncrementally(i); ++i)
	{
	}
}

bool ShaderLoader::LoadShadersIncrementally(int i)
{
	if (i >= ARRAY_LEN(shaderDependencies))
	{
		return false;
	}
	const ShaderDependency& info = shaderDependencies[i];
	m_shaders[info.name] = m_gfxLayer->CreateShader();
	m_gfxLayer->LoadShader(m_shaders[info.name], shaderSources[info.vs], NULL, shaderSources[info.fs], NULL);
	return true;
}

ShaderID ShaderLoader::GetShader(const ShaderName& name) const
{
	return m_shaders[name];
}

#endif // !SHADER_EDIT
