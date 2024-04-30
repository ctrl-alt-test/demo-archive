#ifndef DRAW_AREA_HH
#define DRAW_AREA_HH

#include "ResourceID.hh"

namespace Gfx
{
	struct Viewport
	{
		int				x;
		int				y;
		int				width;
		int				height;

		inline bool operator == (const Viewport& rhs) const
		{
			return
				x == rhs.x &&
				y == rhs.y &&
				width == rhs.width &&
				height == rhs.height;
		}

		inline bool operator != (const Viewport& rhs) const
		{
			return !(*this == rhs);
		}
	};

	/// <summary>
	/// Destination frame buffer and viewport information.
	/// </summary>
	struct DrawArea
	{
		FrameBufferID	frameBuffer;
		Viewport		viewport;
	};
}

#endif // DRAW_AREA_HH
