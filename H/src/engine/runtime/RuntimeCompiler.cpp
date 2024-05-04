#ifdef ENABLE_RUNTIME_COMPILATION

#include "RuntimeCompiler.hh"
#include "engine/core/Debug.hh"
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/RuntimeObjectSystem.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>

using namespace runtime;

void RuntimeCompiler::Logger::LogError(const char * format, ...)
{
#ifdef ENABLE_LOG
	Core::LogLevel::Enum logLevel = Core::LogLevel::Error;
	if (strcmp(format, "%s") == 0)
	{
		// Assume the log to come from the compiler: output as is.
		logLevel = Core::LogLevel::Raw;
	}
	va_list argList;
	va_start(argList, format);
	Core::log.AddList(logLevel, format, argList);
	va_end(argList);
#endif // ENABLE_LOG
}

void RuntimeCompiler::Logger::LogWarning(const char * format, ...)
{
#ifdef ENABLE_LOG
	va_list argList;
	va_start(argList, format);
	Core::log.AddList(Core::LogLevel::Warning, format, argList);
	va_end(argList);
#endif // ENABLE_LOG
}

void RuntimeCompiler::Logger::LogInfo(const char * format, ...)
{
#ifdef ENABLE_LOG
	Core::LogLevel::Enum logLevel = Core::LogLevel::Info;
	if (strcmp(format, "%s") == 0)
	{
		// Assume the log to come from the compiler: output as is.
		logLevel = Core::LogLevel::Raw;
	}
	va_list argList;
	va_start(argList, format);
	Core::log.AddList(logLevel, format, argList);
	va_end(argList);
#endif // ENABLE_LOG
}

RuntimeCompiler::RuntimeCompiler()
{
	m_runtimeObjectSystem = new RuntimeObjectSystem();
	m_runtimeObjectSystem->Initialise(&m_logger, NULL);

	FileSystemUtils::Path srcDir = m_runtimeObjectSystem->FindFile(__FILE__);
	srcDir = srcDir.ParentPath().ParentPath().ParentPath();

	m_runtimeObjectSystem->AddIncludeDir(srcDir.c_str());
	m_runtimeObjectSystem->AddIncludeDir((srcDir.ParentPath() / FileSystemUtils::Path("thirdparty")).c_str());

	// Preprocessor definitions for debug mode and path to libraries.
#ifdef _WIN32
	m_runtimeObjectSystem->SetAdditionalCompileOptions(
		"/D _DEBUG "
		"/D DEBUG "
		"/D DLL_COMPILATION "
		"/D ENABLE_LOG "
		"/D ENABLE_RUNTIME_COMPILATION "
		"/D GFX_MULTI_API=1 "
		"/D _HAS_EXCEPTIONS=1 "
		"\"project\\VS2012\\Debug\\RuntimeCompiler_VS2012.lib\" "
		"\"project\\VS2012\\Debug\\RuntimeObjectSystem_VS2012.lib\" "
		"\"build\\Win32\\Engine\\Debug\\Engine_d.lib\" "
		"\"build\\Win32\\GraphicLayer\\Debug\\GraphicLayer_d.lib\""
		);
#else // !_WIN32
	m_runtimeObjectSystem->AddLibraryDir((srcDir.ParentPath() / FileSystemUtils::Path("build/Linux/debug")).c_str());
	m_runtimeObjectSystem->SetAdditionalCompileOptions(
		"-std=c++11 "
		"-D_DEBUG "
		"-DDEBUG "
		"-DDLL_COMPILATION "
		"-DENABLE_LOG "
		"-DENABLE_RUNTIME_COMPILATION "
		"-DGFX_MULTI_API=1 "
		"-D_HAS_EXCEPTIONS=1 "
		"-lruntimecompiler "
		"-lruntimeobjectsystem "
		"-lplatform "
		"-lgfx "
		"-lengine "
		);
#endif // !_WIN32
}

RuntimeCompiler::~RuntimeCompiler()
{
	m_runtimeObjectSystem->CleanObjectFiles();
	delete m_runtimeObjectSystem;
}

void RuntimeCompiler::AddListener(IObjectFactoryListener* listener) const
{
	m_runtimeObjectSystem->GetObjectFactorySystem()->AddListener(listener);
}

void RuntimeCompiler::RemoveListener(IObjectFactoryListener* listener) const
{
	m_runtimeObjectSystem->GetObjectFactorySystem()->RemoveListener(listener);
}

void RuntimeCompiler::Update(int dt) const
{
	m_runtimeObjectSystem->GetFileChangeNotifier()->Update(dt / 1000.f);
	if (m_runtimeObjectSystem->GetIsCompiledComplete())
	{
		m_runtimeObjectSystem->LoadCompiledModule();
	}
}

void RuntimeCompiler::LoadCompiledCode(const char* type, const InterfaceID& interfaceId, void** out_instance, ObjectId* out_objectId) const
{
	assert(type != nullptr);
	assert(out_instance != nullptr);

	IObjectConstructor* constructor = m_runtimeObjectSystem->GetObjectFactorySystem()->GetConstructor(type);
	if (constructor != nullptr)
	{
		IObject* pObj = constructor->Construct();
		pObj->GetInterface(interfaceId, out_instance);
		if (*out_instance != nullptr)
		{
			*out_objectId = pObj->GetObjectId();
		}
		else
		{
			delete pObj;
			LOG_ERROR("No interface found for %s.", type);
		}
	}
	else
	{
		LOG_ERROR("No constructor found for %s.", type);
	}
}

void RuntimeCompiler::LoadCompiledCode(CompilationData& object) const
{
	if (object.type != nullptr)
	{
		LoadCompiledCode(object.type, object.interfaceId, object.currentInstance, &object.objectId);
		object.hasCodeChanged = true;
	}
}

bool RuntimeCompiler::UpdateCompiledCode(const InterfaceID& interfaceId, void** instance, const ObjectId& objectId) const
{
	assert(instance != nullptr && *instance != nullptr);

	const void* previousInstance = *instance;
	IObject* pObj = m_runtimeObjectSystem->GetObjectFactorySystem()->GetObject(objectId);
	pObj->GetInterface(interfaceId, (void**)instance);
	if (*instance == nullptr)
	{
		delete pObj;
		LOG_ERROR("No interface found for %s.", pObj->GetTypeName());
	}
	else if (previousInstance != *instance)
	{
		LOG_INFO("%s's code has changed.", pObj->GetTypeName());
		return true;
	}
	return false;
}

bool RuntimeCompiler::UpdateCompiledCode(CompilationData& object) const
{
	if (object.type != nullptr)
	{
		object.hasCodeChanged = UpdateCompiledCode(object.interfaceId, object.currentInstance, object.objectId);
	}
	return object.hasCodeChanged;
}

#endif // ENABLE_RUNTIME_COMPILATION
