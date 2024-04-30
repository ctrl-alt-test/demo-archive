#ifndef IMESH_BUILDER_HH
#define IMESH_BUILDER_HH

#ifdef ENABLE_RUNTIME_COMPILATION

#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IObject.h"

namespace mesh
{
	struct Mesh;
	struct Pool;

	/// <summary>
	/// Interface for a mesh building object that can be recompiled at
	/// runtime, using RuntimeCompiledC++.
	/// </summary>
	struct IMeshBuilder : public IObject
	{
		virtual void Build(mesh::Mesh& dest, mesh::Pool& pool) = 0;
	};
}

#endif // ENABLE_RUNTIME_COMPILATION

#endif // IMESH_BUILDER_HH
