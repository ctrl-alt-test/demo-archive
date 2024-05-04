#ifndef BLENDING_MODE_HH
#define BLENDING_MODE_HH

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <GL/gl.h>
#include "gfx/OpenGL/glext.h"

namespace Gfx
{
	struct BlendEquation
	{
		enum Enum {
			SrcPlusDst = GL_FUNC_ADD,
			SrcMinusDst = GL_FUNC_SUBTRACT,
			DstMinusSrc = GL_FUNC_REVERSE_SUBTRACT,
			Min = GL_MIN,
			Max = GL_MAX,
		};
	};

	struct BlendFunction
	{
		enum Enum {
			Zero = GL_ZERO,
			One = GL_ONE,
			SrcColor = GL_SRC_COLOR,
			OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
			DstColor = GL_DST_COLOR,
			OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
			SrcAlpha = GL_SRC_ALPHA,
			OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
			DstAlpha = GL_DST_ALPHA,
			OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
			Factor = GL_CONSTANT_COLOR,
			OneMinusFactor = GL_ONE_MINUS_CONSTANT_COLOR,
			AlphaSaturate = GL_SRC_ALPHA_SATURATE,
		};
	};

	/// <summary>
	/// All the parameters for the blending equation.
	/// </summary>
	struct BlendingMode
	{
		static const BlendingMode Opaque;
		static const BlendingMode Translucent;
		static const BlendingMode Additive;

		BlendEquation::Enum	rgbEquation;
		BlendEquation::Enum	alphaEquation;

		BlendFunction::Enum	srcRGBFunction;
		BlendFunction::Enum	srcAlphaFunction;
		BlendFunction::Enum	dstRGBFunction;
		BlendFunction::Enum	dstAlphaFunction;

		BlendingMode():
			rgbEquation(BlendEquation::SrcPlusDst),
			alphaEquation(BlendEquation::SrcPlusDst),
			srcRGBFunction(BlendFunction::One),
			srcAlphaFunction(BlendFunction::One),
			dstRGBFunction(BlendFunction::Zero),
			dstAlphaFunction(BlendFunction::Zero)
		{}

		BlendingMode(BlendEquation::Enum rgbEquation,
					 BlendEquation::Enum alphaEquation,
					 BlendFunction::Enum srcRGBFunction,
					 BlendFunction::Enum srcAlphaFunction,
					 BlendFunction::Enum dstRGBFunction,
					 BlendFunction::Enum dstAlphaFunction):
			rgbEquation(rgbEquation),
			alphaEquation(alphaEquation),
			srcRGBFunction(srcRGBFunction),
			srcAlphaFunction(srcAlphaFunction),
			dstRGBFunction(dstRGBFunction),
			dstAlphaFunction(dstAlphaFunction)
		{}

		inline bool operator == (const BlendingMode& rhs) const
		{
			return
				rgbEquation == rhs.rgbEquation &&
				alphaEquation == rhs.alphaEquation &&
				srcRGBFunction == rhs.srcRGBFunction &&
				srcAlphaFunction == rhs.srcAlphaFunction &&
				dstRGBFunction == rhs.dstRGBFunction &&
				dstAlphaFunction == rhs.dstAlphaFunction;
		}

		inline bool operator != (const BlendingMode& rhs) const
		{
			return !(*this == rhs);
		}
	};
}

#endif // BLENDING_MODE_HH
