#if DEBUG

#include "ImGuiHelper.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Utils.hh"
#include "gfx/DrawArea.hh"
#include "gfx/Geometry.hh"
#include "gfx/IGraphicLayer.hh"
#include "gfx/RasterTests.hh"
#include "gfx/ShadingParameters.hh"
#include "gfx/TextureFormat.hh"
#include "gfx/VertexAttribute.hh"
#include "imgui/imgui.h"
#include "platform/KeyCodes.hh"
#include "tool/KeyboardAndMouseState.hh"
#include "tool/ShaderHelper.hh"

using namespace Tool;

static Gfx::IGraphicLayer*		s_gfxLayer = nullptr;
static Gfx::TextureID			s_imGuiFontTexture = Gfx::TextureID::InvalidID;
static Gfx::ShadingParameters	s_imGuiShading = Gfx::ShadingParameters();
static Gfx::Geometry			s_imGuiGeometry = {
	Gfx::VertexBufferID::InvalidID,
	0,
	0,
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
	0,
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
};

static const Gfx::VertexAttribute s_imGuiVertexAttributes[] = {
	{ U_VERTEXPOSITION,	2, Gfx::VertexAttributeType::Float,			},
	{ U_VERTEXTEXCOORD,	2, Gfx::VertexAttributeType::Float,			},
	{ U_VERTEXCOLOR,	4, Gfx::VertexAttributeType::UnsignedByte,	},
};

void ImGuiHelper::Init(Gfx::IGraphicLayer* gfxLayer)
{
	s_gfxLayer = gfxLayer;

	ImGuiIO& io = ImGui::GetIO();
	io.RenderDrawListsFn = ImGuiHelper::RenderDrawList;
	//io.SetClipboardTextFn = ImGuiHelper::SetClipboardText;
	//io.GetClipboardTextFn = ImGuiHelper::GetClipboardText;

	// Temporary hack because the mouse coordinates are a bit off:
	io.MouseDrawCursor = true;

	// Keyboard mapping. ImGui will use those indices to peek into the
	// io.KeyDown[] array.
	{
		io.KeyMap[ImGuiKey_Tab] =        platform::KeyCode::keyTab;
		io.KeyMap[ImGuiKey_LeftArrow] =  platform::KeyCode::keyArrowLeft;
		io.KeyMap[ImGuiKey_RightArrow] = platform::KeyCode::keyArrowRight;
		io.KeyMap[ImGuiKey_UpArrow] =    platform::KeyCode::keyArrowUp;
		io.KeyMap[ImGuiKey_DownArrow] =  platform::KeyCode::keyArrowDown;
		io.KeyMap[ImGuiKey_PageUp] =     platform::KeyCode::keyPageUp;
		io.KeyMap[ImGuiKey_PageDown] =   platform::KeyCode::keyPageDown;
		io.KeyMap[ImGuiKey_Home] =       platform::KeyCode::keyHome;
		io.KeyMap[ImGuiKey_End] =        platform::KeyCode::keyEnd;
		io.KeyMap[ImGuiKey_Delete] =     platform::KeyCode::keyDelete;
		io.KeyMap[ImGuiKey_Backspace] =  platform::KeyCode::keyBackspace;
		io.KeyMap[ImGuiKey_Enter] =      platform::KeyCode::keyEnter;
		io.KeyMap[ImGuiKey_Escape] =     platform::KeyCode::keyEscape;
		io.KeyMap[ImGuiKey_A] =          platform::KeyCode::keyA;
		io.KeyMap[ImGuiKey_C] =          platform::KeyCode::keyC;
		io.KeyMap[ImGuiKey_V] =          platform::KeyCode::keyV;
		io.KeyMap[ImGuiKey_X] =          platform::KeyCode::keyX;
		io.KeyMap[ImGuiKey_Y] =          platform::KeyCode::keyY;
		io.KeyMap[ImGuiKey_Z] =          platform::KeyCode::keyZ;
	}

	// Shader.
	{
		const char* vertexShader =
			"#version 330\n"
			"uniform mat4 projectionMatrix;\n"
			"layout(location = 0) in vec2 vertexPosition;\n"
			"layout(location = 1) in vec2 vertexTexCoord;\n"
			"layout(location = 2) in vec4 vertexColor;\n"
			"out vec2 texCoord;\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"	texCoord = vertexTexCoord;\n"
			"	color = vertexColor / 255.;\n"
			"	gl_Position = projectionMatrix * vec4(vertexPosition, 0.0, 1.0);\n"
			"}\n";

		const char* fragmentShader =
			"#version 330\n"
			"uniform sampler2D texture0;\n"
			"in vec2 texCoord;\n"
			"in vec4 color;\n"
			"out vec4 fragmentColor;\n"
			"void main()\n"
			"{\n"
			"	fragmentColor = color * texture(texture0, texCoord).r;\n"
			"}\n";
		s_imGuiShading.shader = s_gfxLayer->LoadShader(s_gfxLayer->CreateShader(),
			vertexShader, __FILE__,
			fragmentShader, __FILE__);
	}

	// Font texture.
	{
		unsigned char* pixels;
		int fontTextureWidth;
		int fontTextureHeight;
		io.Fonts->GetTexDataAsAlpha8(&pixels, &fontTextureWidth, &fontTextureHeight);
		Gfx::TextureSampling sampling = {
			Gfx::TextureFilter::Linear,
			Gfx::TextureFilter::Linear,
			1.f,
			Gfx::TextureWrap::ClampToEdge,
			Gfx::TextureWrap::ClampToEdge,
			Gfx::TextureWrap::ClampToEdge,
		};

		s_imGuiFontTexture = s_gfxLayer->LoadTexture(s_gfxLayer->CreateTexture(),
			fontTextureWidth, fontTextureHeight,
			Gfx::TextureType::Texture2D, Gfx::TextureFormat::R8, 0, 0,
			pixels, sampling);
		io.Fonts->TexID = (ImTextureID)&s_imGuiFontTexture;
	}

	// Vertex buffer for the UI geometry.
	{
		s_imGuiGeometry.vertexBuffer = s_gfxLayer->CreateVertexBuffer();
	}
}

void ImGuiHelper::Shutdown()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;

	s_gfxLayer->DestroyShader(s_imGuiShading.shader);
	s_imGuiShading.shader = Gfx::ShaderID::InvalidID;

	s_gfxLayer->DestroyTexture(s_imGuiFontTexture);
	s_imGuiFontTexture = Gfx::TextureID::InvalidID;

	s_gfxLayer->DestroyVertexBuffer(s_imGuiGeometry.vertexBuffer);
	s_imGuiGeometry.vertexBuffer = Gfx::VertexBufferID::InvalidID;

	ImGui::Shutdown();
}

void ImGuiHelper::NewFrame(int renderWidth, int renderHeight, float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)renderWidth, (float)renderHeight);
	io.DisplayFramebufferScale = ImVec2(1.f, 1.f);
	io.DeltaTime = deltaTime;
	//io.MousePos = ...;
	//io.MouseDown[i] = ...;

	ImGui::NewFrame();

	io.MouseWheel = 0.f;
}

void ImGuiHelper::RenderDrawList(ImDrawData* drawData)
{
	ImGuiIO& io = ImGui::GetIO();

	Gfx::DrawArea directToScreen = { { -1 }, { 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y } };
	const Algebra::matrix4 projectionMatrix = Algebra::matrix4::ortho(0.0f, io.DisplaySize.x, 0.0f, io.DisplaySize.y, -1.0f, 1.0f);

	s_imGuiShading.blendingMode = Gfx::BlendingMode::Translucent;
	s_imGuiShading.uniforms.empty();
	s_imGuiShading.uniforms.add(Tool::ShaderHelper::CreateMatrixShaderParam(U_PROJECTIONMATRIX, projectionMatrix));
	s_imGuiShading.uniforms.add(Gfx::Uniform::Sampler1(U_TEXTURE0, Gfx::TextureID::InvalidID));

	for (int j = 0; j < drawData->CmdListsCount; ++j)
	{
		const ImDrawList* list = drawData->CmdLists[j];
		s_gfxLayer->LoadVertexBuffer(s_imGuiGeometry.vertexBuffer,
									 s_imGuiVertexAttributes,
									 ARRAY_LEN(s_imGuiVertexAttributes),
									 sizeof(list->VtxBuffer.Data[0]),
									 list->VtxBuffer.Size * sizeof(list->VtxBuffer.Data[0]), (void*)list->VtxBuffer.Data,
									 list->IdxBuffer.Size * sizeof(list->IdxBuffer.Data[0]), (void*)list->IdxBuffer.Data,
									 (sizeof(ImDrawIdx) == 2 ? Gfx::VertexIndexType::UInt16 : Gfx::VertexIndexType::UInt32));

#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		s_imGuiGeometry.firstIndexOffset = 0;

		for (int i = 0; i < list->CmdBuffer.Size; ++i)
		{
			const ImDrawCmd* command = &list->CmdBuffer[i];
			if (command->UserCallback)
			{
				command->UserCallback(list, command);
			}
			else
			{
				Gfx::RasterTests rasterTest(Gfx::FaceCulling::None, Gfx::DepthFunction::Always, false);
#if GFX_ENABLE_SCISSOR_TESTING
				rasterTest.scissorTestEnabled = true;
				rasterTest.scissorX = (int)command->ClipRect.x;
				rasterTest.scissorY = (int)(io.DisplaySize.y - command->ClipRect.w);
				rasterTest.scissorWidth = (int)(command->ClipRect.z - command->ClipRect.x);
				rasterTest.scissorHeight = (int)(command->ClipRect.w - command->ClipRect.y);
#endif // GFX_ENABLE_SCISSOR_TESTING

				s_imGuiGeometry.numberOfIndices = command->ElemCount;
				s_imGuiShading.uniforms.last().id = *(Gfx::TextureID*)command->TextureId;
				s_gfxLayer->Draw(directToScreen, rasterTest, s_imGuiGeometry, s_imGuiShading);
			}
			s_imGuiGeometry.firstIndexOffset += command->ElemCount * sizeof(ImDrawIdx);
		}
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	}
}

bool ImGuiHelper::HandleCharacter(unsigned short character)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(character);
	return io.WantTextInput;
}

bool ImGuiHelper::HandleKey(platform::KeyCode::Enum key, bool pressed)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = pressed;

	if (key == platform::KeyCode::keyLeftControl || key == platform::KeyCode::keyRightControl)
	{
		io.KeyCtrl = pressed;
	}

	if (key == platform::KeyCode::keyLeftShift || key == platform::KeyCode::keyRightShift)
	{
		io.KeyShift = pressed;
	}

	if (key == platform::KeyCode::keyLeftAlt || key == platform::KeyCode::keyRightAlt)
	{
		io.KeyAlt = pressed;
	}

	if (key == platform::KeyCode::keyLeftCommand || key == platform::KeyCode::keyRightCommand)
	{
		io.KeySuper = pressed;
	}

	io.KeySuper = false;

	return io.WantCaptureKeyboard;
}

bool ImGuiHelper::HandleMouse(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(float(x), float(y));
	io.MouseWheel += wheel;
	io.MouseDown[0] = leftDown;
	io.MouseDown[1] = rightDown;
	io.MouseDown[2] = middleDown;
	io.MouseDown[3] = x1Down;
	io.MouseDown[4] = x2Down;

	return io.WantCaptureMouse;
}

#endif // !DEBUG
