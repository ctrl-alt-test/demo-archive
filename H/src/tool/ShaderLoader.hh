//
// This is not part of the engine because it will generate code
// specific to each demo (include of generated list of shaders).
//

#ifndef SHADER_LOADER_HH
#define SHADER_LOADER_HH

#include "IncludeUtils.hh"
#include "gfx/ResourceID.hh"

#if DEBUG
# define SHADER_EDIT 1
#else // !DEBUG
# define SHADER_EDIT 0
#endif // !DEBUG

#if SHADER_EDIT
# include "tool/ShaderMinifier.hh"
# include <string>
# include <vector>
#endif // !SHADER_EDIT

namespace Gfx
{
	class IGraphicLayer;
}

namespace runtime
{
	class FileWatcher;
}

namespace Tool
{
	class Minifier;
}

namespace Tool
{
/// <summary>
/// LOAD_SHADER stores in dest the Gfx::ShaderID of the shader defined
/// by the files fs and vs.
/// In Debug it loads the files and compiles the shader;
/// in release it only gets the shader handle as shaders are assumed to
/// have been compiled already.
/// </summary>
#define LOAD_SHADER(loader, dest, name, vs, fs) LOAD_SHADER_IMPLEM(loader, dest, name, vs, fs)

#if SHADER_EDIT
# define LOAD_SHADER_IMPLEM(loader, dest, name, vsFileName, fsFileName)	do { (*dest) = (loader).CreateLoadAndMonitorShader(#name, vsFileName, fsFileName, LOCATION_IN_SOURCE); } while (false)
#else // !SHADER_EDIT
# include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_shaderNames.hh)
# define LOAD_SHADER_IMPLEM(loader, dest, name, vsFileName, fsFileName)	do { (*dest) = (loader).GetShader(::Tool::name); } while (false)
#endif // !SHADER_EDIT

#if SHADER_EDIT
	struct ShaderInfo
	{
		int				hash;
		std::string		name;
		Gfx::ShaderID	id;
		int				vsIndex;
		int				fsIndex;
		std::string		vsFileName;
		std::string		fsFileName;
		std::string		loadLocation;
	};
#endif // SHADER_EDIT

	class ShaderLoader
	{
	public:
		ShaderLoader(Gfx::IGraphicLayer* gfxLayer,
					 runtime::FileWatcher* fileWatcher,
					 bool minifyAndExportOnDestroy = true);
		~ShaderLoader();

#if SHADER_EDIT
		/// <summary>
		/// Loads and compiles a shader from source files, then
		/// monitors those files and their dependencies. When a file is
		/// modified, the shader is recompiled without invalidating its
		/// id.
		/// </summary>
		Gfx::ShaderID CreateLoadAndMonitorShader(const char* name, const char* vsFileName, const char* fsFileName, const char* callLocation);
#else // !SHADER_EDIT

		/// <summary>
		/// Loads and compiles all the shaders.
		/// </summary>
		void LoadShaders();

		/// <summary>
		/// Loads and compiles the shaders, one at a time.
		/// </summary>
		///
		/// <returns>True if there are shaders left to compile, false
		/// otherwise.</returns>
		bool LoadShadersIncrementally(int i);

		Gfx::ShaderID GetShader(const ShaderName& name) const;
#endif // !SHADER_EDIT

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;

#if SHADER_EDIT
		void LoadAndMonitorShader(size_t index, const char* vs, const char* fs);
		void ReloadAndMonitorShader(size_t index, const char* vs, const char* fs, const char*);

		runtime::FileWatcher*	m_fileWatcher;
		Tool::Minifier			m_minifier;
		bool					m_minifyAndExportOnDestroy;

	public:
		std::vector<ShaderInfo>	m_loadedShaders;
		std::vector<std::pair<ShaderInfo, std::string> > m_shaderAliases;
#else // !SHADER_EDIT
		Gfx::ShaderID			m_shaders[NumberOfShaders];
#endif // !SHADER_EDIT
	};
}

#endif // SHADER_LOADER_HH
