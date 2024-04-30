#ifndef QUAD_HH
#define QUAD_HH

#include "gfx/Geometry.hh"

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
	Gfx::Geometry LoadQuadMesh(Gfx::IGraphicLayer* graphicsLayer);
}

#endif // QUAD_HH
