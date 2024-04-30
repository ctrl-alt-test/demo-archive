#ifndef IMGUI_HELPER_HH
#define IMGUI_HELPER_HH

#if DEBUG

#include "platform/KeyCodes.hh"

struct ImDrawData;

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
	struct ImGuiHelper
	{
		static void Init(Gfx::IGraphicLayer* gfxLayer);
		static void Shutdown();
		static void NewFrame(int renderWidth, int renderHeight, float deltaTime);
		static void RenderDrawList(ImDrawData* drawData);

		static bool HandleCharacter(unsigned short character);
		static bool HandleKey(platform::KeyCode::Enum key, bool pressed);
		static bool HandleMouse(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down);
	};
}

#endif // !DEBUG

#endif // IMGUI_HELPER_HH
