//
// This is not part of the engine because it will generate code
// specific to each demo (include of generated list of variables).
//

#ifndef PROJECT_LOADER_HH
#define PROJECT_LOADER_HH

#include "IncludeUtils.hh"
#include "engine/container/Dict.hh"
#include "engine/timeline/Variable.hh"

#if DEBUG
# define VAR_EDIT 1
#else // !DEBUG
# define VAR_EDIT 0
#endif // !DEBUG

namespace runtime
{
	class FileWatcher;
}

namespace Tool
{
	class ProjectLoader
	{
	public:
		explicit ProjectLoader(runtime::FileWatcher* fileWatcher);
		~ProjectLoader();

#if VAR_EDIT
		void LoadAndMonitorProject(const char* fileName);
		void ReloadAndMonitorProject(const char* fileName, const char*);

		static void readInput(const char* input);
#else // !VAR_EDIT
		void LoadProject();
#endif // !VAR_EDIT

	private:
#if VAR_EDIT
		runtime::FileWatcher*	m_fileWatcher;
#endif // !VAR_EDIT
	};

#if VAR_EDIT
	/// <summary>
	/// Global dictionary containing all variables, by name.
	/// </summary>
	extern Container::Dict<const char*, Timeline::Variable> allVariables;
#define VAR(variableName) (::Tool::allVariables[#variableName])
#else // !VAR_EDIT
#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_variableId.hh)
	/// <summary>
	/// Global array containing all variables.
	/// </summary>
	extern Timeline::Variable allVariables[numberOfVariables];
#define VAR(variableName) (&::Tool::allVariables[::Tool::variableName])
#endif // !VAR_EDIT
}

#endif // PROJECT_LOADER_HH
