#ifndef ITEXTURE_BUILDER_HH
#define ITEXTURE_BUILDER_HH

#ifdef ENABLE_RUNTIME_COMPILATION

#include "engine/algebra/Vector2.hh"
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IObject.h"
#include <cassert>

namespace texture
{
	class Texture;

	/// <summary>
	/// Interface for a texture building object that can be recompiled
	/// at runtime, using RuntimeCompiledC++.
	/// </summary>
	struct ITextureBuilder : public IObject
	{
		virtual void BuildPipe(texture::Texture& /*dest*/)
		{
			assert(false);
		}

		virtual void Build(void* /*out_pixelData*/,
						   const void* /*inputPixelData*/,
						   int /*width*/,
						   int /*height*/,
						   int /*i*/,
						   int /*j*/,
						   const Algebra::vector2f& /*coord*/)
		{
			assert(false);
		}
	};
}

#endif // ENABLE_RUNTIME_COMPILATION

#endif // ITEXTURE_BUILDER_HH
