#include "LoadingBar.hh"

#undef Always
#include "engine/algebra/Functions.hh"
#include "engine/core/Debug.hh"
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/Quad.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"

#ifndef _WIN32
# include <stdlib.h> // for exit()
#endif

#if !DEBUG
#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_loadingSteps.hh)
#else // DEBUG

// FIXME: Load from the exported file if it exists.
#define NUMBER_OF_LOADING_STEPS 100

void ExportLoadingSteps(const char* fileName, const char* macroName, int steps)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', shaders ids won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "#define NUMBER_OF_LOADING_STEPS\t%d\n", steps);
		fprintf(fp, "\n#endif // %s\n", macroName);
		fclose(fp);
	}
}
#endif // DEBUG

using namespace Gfx;

namespace LoadingBar
{
	// Using global variables makes the code simpler (no need to pass a LoadingBar
	// object to every class).
	static IGraphicLayer*		g_gfxLayer;
	static platform::Platform*	g_platform;
	static int					g_step = 0;
	static ShadingParameters	g_loadingShader;
	static Geometry				g_quad;
	static DrawArea				g_directToScreen = { { -1 }, { 0, 0, 0, 0} };

	void init(IGraphicLayer* gfxLayer,
			   platform::Platform* platform,
			   int windowWidth,
			   int windowHeight)
	{
		g_gfxLayer = gfxLayer;
		g_platform = platform;
		g_quad = Tool::LoadQuadMesh(g_gfxLayer);
		g_loadingShader = ShadingParameters();
		g_directToScreen.viewport.width = windowWidth;
		g_directToScreen.viewport.height = windowHeight;
	}

	void update(void* shaderLoader)
	{
		if (!g_platform->HandleMessages())
		{
			exit(0);
		}

		// HACK: This is not pretty, but it was the fastest way to get
		// it done. Improvements are welcomed.
		if (shaderLoader != nullptr &&
			g_loadingShader.shader == ShaderID::InvalidID)
		{
			LOAD_SHADER(*(Tool::ShaderLoader*)shaderLoader, &g_loadingShader.shader, LoadingShader,
				"assets/shaders/screenspace/blitShader.vs",
				"assets/shaders/screenspace/simpleProgressBar.fs");
		}

		if (g_step < 0)
		{
			return; // loading is complete, ignore this update.
		}
		++g_step;

		// Draw the loading progress.
		{
			float progress = float(g_step) / float(NUMBER_OF_LOADING_STEPS);
			if (g_loadingShader.shader != ShaderID::InvalidID)
			{
				g_loadingShader.uniforms.empty();
				Tool::ShaderHelper::ExposeFullScreenBlitTransform(g_loadingShader.uniforms);
				g_loadingShader.uniforms.add(Uniform::Float1(U_PROGRESS, progress));
				g_gfxLayer->Draw(g_directToScreen, RasterTests::NoDepthTest, g_quad, g_loadingShader);
			}

			g_gfxLayer->EndFrame();
			g_platform->SwapBuffers();
		}
	}

	void complete()
	{
		LOG_INFO("%d loading steps.", g_step);

#if DEBUG
		ExportLoadingSteps("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_loadingSteps.hh), "EXPORTED_LOADING_STEPS", g_step);
#endif // DEBUG

		g_step = -1;
	}
}
