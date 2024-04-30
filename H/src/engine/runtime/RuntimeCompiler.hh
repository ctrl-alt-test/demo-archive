#ifndef RUNTIME_COMPILER_HH
#define RUNTIME_COMPILER_HH

#ifdef ENABLE_RUNTIME_COMPILATION

#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IObject.h"
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IObjectFactorySystem.h"
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IRuntimeObjectSystem.h"

namespace runtime
{
	enum RCCIds
	{
		IID_ITextureBuilder = IID_ENDInterfaceID,
		IID_IMeshBuilder,

		RCCIds_MAX
	};

	struct CompilationData
	{
		const char*				type;
		InterfaceID				interfaceId;
		void**					currentInstance;
		ObjectId				objectId;
		bool					hasCodeChanged;
	};

	class RuntimeCompiler
	{
	public:
		RuntimeCompiler();
		~RuntimeCompiler();

		void AddListener(IObjectFactoryListener* listener) const;
		void RemoveListener(IObjectFactoryListener* listener) const;

		void Update(int dt) const;
		void LoadCompiledCode(const char* type, const InterfaceID& interfaceId, void** out_instance, ObjectId* out_objectId) const;
		void LoadCompiledCode(CompilationData& compilationData) const;
		bool UpdateCompiledCode(const InterfaceID& interfaceId, void** instance, const ObjectId& objectId) const;
		bool UpdateCompiledCode(CompilationData& compilationData) const;

	private:
		struct Logger: public ICompilerLogger
		{
			virtual void LogError(const char * format, ...);
			virtual void LogWarning(const char * format, ...);
			virtual void LogInfo(const char * format, ...);
		};

		IRuntimeObjectSystem*	m_runtimeObjectSystem;
		Logger					m_logger;
	};
}

#else // !ENABLE_RUNTIME_COMPILATION

namespace runtime
{
	class RuntimeCompiler;
}

#endif // !ENABLE_RUNTIME_COMPILATION

#endif // RUNTIME_COMPILER_HH
