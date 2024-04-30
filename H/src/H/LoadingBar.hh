#pragma once

#include "engine/noise/Rand.hh"
#include "gfx/OpenGL/OpenGLLayer.hh"
#include "platform/Platform.hh"
#include "tweakval/tweakval.h"

namespace LoadingBar
{
	void init(Gfx::IGraphicLayer* gfxLayer, platform::Platform* platform, int windowWidth, int windowHeight);
	void update(void* shaderLoader = nullptr);
	void complete();
}
