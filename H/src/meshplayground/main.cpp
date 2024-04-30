#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/Light.hh"
#include "engine/core/RigidTransform.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/RevolutionFunctions.hh"
#include "engine/mesh/Utils.hh"
#include "engine/runtime/FileWatcher.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "gfx/OpenGL/OpenGLLayer.hh"
#include "meshes/Artemision.hh"
#include "meshes/Dolphin.hh"
#include "meshes/ElderManStatue.hh"
#include "meshes/Fence.hh"
#include "meshes/GreekColumn.hh"
#include "meshes/Parthenon.hh"
#include "meshes/PlaygroundMesh.hh"
#include "meshes/PoseidonSeated.hh"
#include "meshes/ResearchVessel.hh"
#include "meshes/SdfTest.hh"
#include "meshes/Shark.hh"
#include "meshes/Submersible.hh"
#include "meshes/Statue.hh"
#include "meshes/Stairs.hh"
#include "meshes/Tholos.hh"
#include "meshes/Trident.hh"
#include "meshes/Viaduct.hh"
#include "meshplayground/OrbitCameraControls.hh"
#include "postprocessing/CommonPostProcessing.hh"
#include "textures/DebugTextures.hh"
#include "textures/GreekColumn.hh"
#include "textures/GreekTiles.hh"
#include "textures/Marble.hh"
#include "textures/MaterialStudy.hh"
#include "textures/Mosaic.hh"
#include "textures/NoiseTextures.hh"
#include "textures/OldStone.hh"
#include "textures/RCCTest.hh"
#include "textures/ResearchVessel.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/KeyboardAndMouseState.hh"
#include "tool/mesh/MeshHelper.hh"
#include "tool/mesh/VertexDataPNT.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/Quad.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include "tool/TextureLoader.hh"
#include "tweakval/tweakval.h"
#include <cassert>

#if !MESH_EDIT || !SHADER_EDIT || !TEXTURE_EDIT
#error The MeshPlayground is meant to be compiled in EDIT mode.
#endif // !MESH_EDIT || !SHADER_EDIT || !TEXTURE_EDIT

#define MAX_MESH_COMPONENTS 64

using namespace Tool;

namespace LoadingBar {
	void update(void*) {}
}

struct MeshInfo
{
	Tool::RecompilableMeshBuilder	meshBuilder;
	const char*						vertexShader;   // Leave NULL to use the default white shader.
	const char*						fragmentShader; //
	Tool::CommonMaterial			material;
	Core::RigidTransform			transform;
};

static MeshInfo theMeshesInfo[] = {
	{ RECOMPILABLE_MESH(Viaduct, VertexDataPNTT),
	  NULL, NULL,
	  Tool::CommonMaterial().SetDiffuse(0.5f, 0.5f, 0.5f).SetSpecular(0.02f).SetRoughness(0.8f),
	  Core::RigidTransform::translation(0, 0, 0) },
};

struct TextureInfo
{
	Tool::RecompilableTextureBuilder	textureBuilder;
	float								parallaxScale;
	const char*							vertexShader;
	const char*							fragmentShader;
	Tool::CommonMaterial				material;
};

static TextureInfo theTextureInfo[] = {
	{ RECOMPILABLE_TEXTURE(OldStone, 512, 512), OldStone::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(SubmersibleSpotTex, 512, 512), SubmersibleBodyTex::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbrWithEmissive.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(SubmersibleBodyTex, 512, 512), SubmersibleBodyTex::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(RVHullTex, 512, 512), RVHullTex::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(FineRedAndWhiteMarbleFloor, 512, 512), FineRedAndWhiteMarbleFloor::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(IonicColumn, 512, 512), IonicColumn::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(FineWhiteMarbleWall, 512, 512), FineWhiteMarbleWall::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial()},

	{ RECOMPILABLE_TEXTURE(Orichalcum, 256, 256), Orichalcum::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbrWithEmissive.fs",
	  Tool::CommonMaterial() },

	{ RECOMPILABLE_TEXTURE(ScifiMetalPlate, 256, 256), ScifiMetalPlate::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbrWithEmissive.fs",
	  Tool::CommonMaterial() },

	{ RECOMPILABLE_TEXTURE(GreekTiles, 256, 256), GreekTiles::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial() },

	{ RECOMPILABLE_TEXTURE(Bricks, 256, 256), Bricks::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial() },

	{ RECOMPILABLE_TEXTURE(CanteenTiles, 256, 256), CanteenTiles::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial() },

	{ RECOMPILABLE_TEXTURE(GraniteCobbleStone, 256, 256), GraniteCobbleStone::s_parallaxScale,
	  "assets/shaders/pbr.vs",
	  "assets/shaders/pbr.fs",
	  Tool::CommonMaterial() },
};

#if DEBUG
#include <iostream>
#include <sstream>
#endif

class MeshPlayground
{
	enum EditionMode
	{
		MeshEdit,
		TextureEdit,
	};

	enum TextureVisualization
	{
		MeshTurnTable,
		CubeTurnTable,
		CylinderTurnTable,
		SphereTurnTable,
	};

public:
	MeshPlayground(Gfx::IGraphicLayer* gfxLayer, runtime::FileWatcher* fileWatcher, runtime::RuntimeCompiler* runtimeCompiler):
		m_gfxLayer(gfxLayer),
		m_cameraControls(),
		m_meshLoader(gfxLayer, runtimeCompiler),
		m_shaderLoader(gfxLayer, fileWatcher, false),
		m_textureLoader(gfxLayer, runtimeCompiler),
		m_postProcessing(gfxLayer),
		m_editionMode(MeshEdit), // Set to MeshEdit or TextureEdit. Until we add a binding.
		m_textureVisualization(CubeTurnTable),
		m_textureId(0)
	{
	}

	void InitTexture(int i)
	{
		TextureInfo& texture = theTextureInfo[i];
		Gfx::TextureID* textureIDs = m_textureLoader.Load(texture.textureBuilder);

		std::ostringstream oss;
		oss << "textureShader_" << i;
		std::string shaderName = oss.str();

		texture.material.shader = m_shaderLoader.CreateLoadAndMonitorShader(shaderName.c_str(), texture.vertexShader, texture.fragmentShader, LOCATION_IN_SOURCE);
		texture.material
			.SetDiffuse(textureIDs[0])
			.SetSpecularAndRoughness(textureIDs[1])
			.SetNormalAndHeight(textureIDs[2]);
		texture.material.parallaxScale = texture.parallaxScale;

		// Hack: deduce from the number of layers which material to use.
		if (texture.textureBuilder.numberOfLayers > 3)
		{
			texture.material.SetEmissive(textureIDs[3]);
		}
	}

	void Init(int renderWidth, int renderHeight)
	{
		m_renderWidth = renderWidth;
		m_renderHeight = renderHeight;
		m_cameraControls.aspectRatio = (float)m_renderWidth / (float)m_renderHeight;
		m_cameraControls.distance = 10.f;
		m_cameraControls.fov = 45.f * DEG_TO_RAD;
		m_cameraControls.distance = 10.f;
		m_cameraControls.latitude = 15.f * DEG_TO_RAD;
		m_cameraControls.longitude = 30.f * DEG_TO_RAD;
		m_cameraControls.anchor.x = 0.f;
		m_cameraControls.anchor.y = 0.f;
		m_cameraControls.anchor.z = 0.f;

		assert(MAX_MESH_COMPONENTS >= ARRAY_LEN(theMeshesInfo));
		for (unsigned int i = 0; i < ARRAY_LEN(theMeshesInfo); ++i)
		{
			MeshInfo& info = theMeshesInfo[i];
			m_theMeshGeometries[i] = m_meshLoader.Load(info.meshBuilder);

			std::stringstream cppIsUglyFront;
			std::stringstream cppIsUglyBack;
			cppIsUglyFront << "frontShader_" << i;
			cppIsUglyBack << "backShader_" << i;
			std::string frontShaderName = cppIsUglyFront.str();
			std::string backShaderName = cppIsUglyBack.str();

			const char* vertexShader = info.vertexShader != NULL ? info.vertexShader : "assets/shaders/unicolor.vs";
			const char* fragmentShader = info.fragmentShader != NULL ? info.fragmentShader : "assets/shaders/unicolor.fs";
			const char* backFaceFragmentShader = "assets/shaders/debug/backFacesView.fs";
			info.material.shader = m_shaderLoader.CreateLoadAndMonitorShader(frontShaderName.c_str(), vertexShader, fragmentShader, LOCATION_IN_SOURCE);
			m_theBackFaceShaders[i] = m_shaderLoader.CreateLoadAndMonitorShader(backShaderName.c_str(), vertexShader, backFaceFragmentShader, LOCATION_IN_SOURCE);
		}

		struct streatchUV
		{
			static mesh::vertex f(int, int, const mesh::vertex& inputVertex)
			{
				mesh::vertex result = inputVertex;
				// Double purpose: make UV look more uniform on the
				// cylinder and sphere, and test tiling below 0 and
				// above 1.
				result.u = Algebra::mix(-1.f, 2.f, result.u);
				return result;
			}
		};

		// The cube mesh for texture showcase.
		mesh::Mesh cube(24);
		mesh::sharpCube(cube).ComputeNormals();
		mesh::GenerateAxisAlignedTextureCoordinates(cube, 1.f);
		cube.Translate(-0.5f, -0.5f, -0.5f).Scale(4.f).ComputeTangents();

		m_theTextureCubeMeshGeometry.vertexBuffer = m_gfxLayer->CreateVertexBuffer();
		m_theTextureCubeMeshGeometry.numberOfInstances = 0;
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		m_theTextureCubeMeshGeometry.firstIndexOffset = 0;
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
		Tool::MeshHelper::LoadVertexBuffer(m_gfxLayer,
										   m_theTextureCubeMeshGeometry.vertexBuffer,
										   cube,
										   Tool::VertexDataPNTT::s_descriptor,
										   &m_theTextureCubeMeshGeometry.numberOfIndices);

		// The cylinder mesh for texture showcase.
		mesh::Mesh cylinder(200);
		mesh::Revolution(mesh::pipeHFunc, mesh::pipeRFunc).GenerateMesh(cylinder, 1, 48);
		cylinder.Translate(0.f, -0.5f, 0.f).Scale(2.f, 4.f, 2.f).ComputeNormals();
		cylinder.ComputeTangents();
		cylinder.ApplyFunction(streatchUV::f);
		m_theTextureCylinderMeshGeometry.vertexBuffer = m_gfxLayer->CreateVertexBuffer();
		m_theTextureCylinderMeshGeometry.numberOfInstances = 0;
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		m_theTextureCylinderMeshGeometry.firstIndexOffset = 0;
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
		Tool::MeshHelper::LoadVertexBuffer(m_gfxLayer,
										   m_theTextureCylinderMeshGeometry.vertexBuffer,
										   cylinder,
										   Tool::VertexDataPNTT::s_descriptor,
										   &m_theTextureCylinderMeshGeometry.numberOfIndices);

		// The sphere mesh for texture showcase.
		mesh::Mesh sphere(5000);
		mesh::Revolution(mesh::sphereHFunc, mesh::sphereRFunc).GenerateMesh(sphere, 24, 48);
		sphere.Scale(2.5f, 2.5f, 2.5f).ComputeNormals();
		sphere.ComputeTangents();
		sphere.ApplyFunction(streatchUV::f);
		m_theTextureSphereMeshGeometry.vertexBuffer = m_gfxLayer->CreateVertexBuffer();
		m_theTextureSphereMeshGeometry.numberOfInstances = 0;
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		m_theTextureSphereMeshGeometry.firstIndexOffset = 0;
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
		Tool::MeshHelper::LoadVertexBuffer(m_gfxLayer,
										   m_theTextureSphereMeshGeometry.vertexBuffer,
										   sphere,
										   Tool::VertexDataPNTT::s_descriptor,
										   &m_theTextureSphereMeshGeometry.numberOfIndices);

		for (unsigned i = 0; i < ARRAY_LEN(theTextureInfo); i++)
		{
			theTextureInfo[i].material.shader = Gfx::ShaderID::InvalidID;
		}

		Gfx::Geometry quad = Tool::LoadQuadMesh(m_gfxLayer);
		Tool::TextureHelper textureHelper(m_gfxLayer);
		m_frameBufferLayers[0] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::R11G11B10f);
		m_frameBufferLayers[1] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::Depth);
		m_colorFrameBuffer = m_gfxLayer->CreateFrameBuffer(m_frameBufferLayers, ARRAY_LEN(m_frameBufferLayers));

		m_postProcessing.Init(m_gfxLayer, quad, &m_shaderLoader, &m_textureLoader,
			m_renderWidth, m_renderHeight,
			m_renderWidth, m_renderHeight);
	}

	void Update(long /*currentTime*/)
	{
		if (theTextureInfo[m_textureId].material.shader == Gfx::ShaderID::InvalidID)
		{
			InitTexture(m_textureId);
		}

		m_camera = m_cameraControls.GetCamera();

		m_lightRig.ambientLight = Algebra::vec3(0.1f);
		m_lightRig.shadowMaps = nullptr;
		const Algebra::vector3f lightColors[] = {
			{ 1.f, 1.f, 1.f, },
			{ 1.f, 1.f, 1.f, },
			{ 1.f, 1.f, 1.f, },
			{ 1.f, 1.f, 1.f, },
			{ 1.f, 1.f, 1.f, },
		};
		assert(ARRAY_LEN(lightColors) == NUM_LIGHTS);

		const Algebra::vector3f lightPositions[] = {
			{ _TV(0.f), _TV(1.f), _TV(-1.f), },
			{ _TV(1.f), _TV(1.f), _TV(1.f), },
			{ _TV(-1.f), _TV(1.f), _TV(1.f), },
			{ _TV(0.f), _TV(-1.f), _TV(0.f), },
			{ _TV(0.5f), _TV(-1.f), _TV(-1.f), },
		};
		assert(ARRAY_LEN(lightPositions) == NUM_LIGHTS);

		for (unsigned int i = 0; i < ARRAY_LEN(m_lightRig.lights); ++i)
		{
			Core::RigidTransform lightTransform = { _TV(35.f) * Algebra::normalized(lightPositions[i]), Algebra::quaternion::identity, };
			Core::Light& light = m_lightRig.lights[i];
			light.transform = lightTransform;

			light.color = _TV(500.f) * lightColors[i];
		}
	}

	void Draw(long currentTime)
	{
		Tool::ParticipatingMedium voidMedium = { Algebra::vector3f::zero, 0.f, 0.f, 0.f };

		const Gfx::DrawArea deferredFrameBuffer = { m_colorFrameBuffer, { 0, 0, m_renderWidth, m_renderHeight } };
		const Gfx::RasterTests defaultTest(m_cameraControls.enableFaceCulling ? Gfx::FaceCulling::Back : Gfx::FaceCulling::None, Gfx::DepthFunction::Less, true);
		const Gfx::RasterTests backFaceRender(Gfx::FaceCulling::Front, Gfx::DepthFunction::Less, true);
		m_gfxLayer->Clear(deferredFrameBuffer.frameBuffer, _TV(0.05f), _TV(0.08f), _TV(0.12f), true);
		m_theShadingParameters.polygonMode = m_cameraControls.wireFrame ? Gfx::PolygonMode::Lines : Gfx::PolygonMode::Filled;

		if (m_editionMode == MeshEdit)
		{
			for (unsigned int i = 0; i < ARRAY_LEN(theMeshesInfo); ++i)
			{
				const Algebra::matrix4 model = Core::ComputeMatrix(theMeshesInfo[i].transform);
				theMeshesInfo[i].material.SetShaderParameters(m_theShadingParameters, model, currentTime, m_camera, m_lightRig, voidMedium, false);

				m_gfxLayer->Draw(deferredFrameBuffer, defaultTest, m_theMeshGeometries[i]->geometry, m_theShadingParameters);

				if (m_cameraControls.enableFaceCulling)
				{
					m_theShadingParameters.shader = m_theBackFaceShaders[i];
					m_gfxLayer->Draw(deferredFrameBuffer, backFaceRender, m_theMeshGeometries[i]->geometry, m_theShadingParameters);
				}
			}
		}
		else
		{
			const Algebra::matrix4 model = Algebra::matrix4::identity;
			theTextureInfo[m_textureId].material.SetShaderParameters(m_theShadingParameters, model, currentTime, m_camera, m_lightRig, voidMedium, false);

			switch (m_textureVisualization)
			{
			case CubeTurnTable:
				m_gfxLayer->Draw(deferredFrameBuffer, defaultTest, m_theTextureCubeMeshGeometry, m_theShadingParameters);
				break;
			case CylinderTurnTable:
				m_gfxLayer->Draw(deferredFrameBuffer, defaultTest, m_theTextureCylinderMeshGeometry, m_theShadingParameters);
				break;
			case SphereTurnTable:
				m_gfxLayer->Draw(deferredFrameBuffer, defaultTest, m_theTextureSphereMeshGeometry, m_theShadingParameters);
				break;
			case MeshTurnTable:
				m_gfxLayer->Draw(deferredFrameBuffer, defaultTest, m_theMeshGeometries[0]->geometry, m_theShadingParameters);
				break;
			}
		}

		// Post-processing
		const Algebra::vector3f one = { 1.f, 1.f, 1.f };
		const Algebra::vector3f bloomFilter = { _TV(0.02f), _TV(0.02f), _TV(0.02f) };
		m_postProcessing.Apply(0,
							   m_frameBufferLayers[0],
							   bloomFilter,
							   Algebra::vector3f::zero,
							   Algebra::vector3f::zero,
							   Algebra::vector3f::zero,
							   Algebra::vector3f::zero, one, one,
							   _TV(2.f),
							   _TV(0.f),
							   _TV(1.0f));
	}

	void OnKeyboard(const platform::KeyEvent& event,
					const Tool::KeyboardState& keyboardState)
	{
		if (event.pressed)
		{
			switch (event.key)
			{
			case platform::KeyCode::keyF1:
				m_editionMode = MeshEdit;
				break;
			case platform::KeyCode::keyF2:
				m_editionMode = TextureEdit;
				m_textureVisualization = MeshTurnTable;
				break;
			case platform::KeyCode::keyF3:
				m_editionMode = TextureEdit;
				m_textureVisualization = CubeTurnTable;
				break;
			case platform::KeyCode::keyF4:
				m_editionMode = TextureEdit;
				m_textureVisualization = CylinderTurnTable;
				break;
			case platform::KeyCode::keyF5:
				m_editionMode = TextureEdit;
				m_textureVisualization = SphereTurnTable;
				break;
			case platform::KeyCode::keyPlus:
			case platform::KeyCode::keyNumpadPlus:
				m_textureId = (m_textureId + 1) % ARRAY_LEN(theTextureInfo);
				break;
			case platform::KeyCode::keyMinus:
			case platform::KeyCode::keyNumpadMinus:
				m_textureId = (m_textureId + ARRAY_LEN(theTextureInfo) - 1) % ARRAY_LEN(theTextureInfo);
				break;
			default:
				break;
			}
		}
		m_cameraControls.OnKeyboard(event, keyboardState);
	}

	void OnMouseButton(const Tool::MouseState& mouseState,
					   const Tool::MouseState& mouseDiff,
					   const Tool::KeyboardState& keyboardState)
	{
		m_cameraControls.OnMouseButton(mouseState, mouseDiff, keyboardState);
	}

	void OnMouseMove(const Tool::MouseState& mouseState,
					 const Tool::MouseState& mouseDiff,
					 const Tool::KeyboardState& keyboardState)
	{
		m_cameraControls.OnMouseMove(mouseState, mouseDiff, keyboardState);
	}

private:
	Gfx::IGraphicLayer*		m_gfxLayer;
	OrbitCameraControls		m_cameraControls;

	Tool::MeshLoader		m_meshLoader;
	Tool::ShaderLoader		m_shaderLoader;
	Tool::TextureLoader		m_textureLoader;

	int						m_renderWidth;
	int						m_renderHeight;
	Core::Camera			m_camera;
	Tool::FixedLightRig		m_lightRig;

	Render::MeshGeometry*	m_theMeshGeometries[MAX_MESH_COMPONENTS];
	Gfx::ShadingParameters	m_theShadingParameters;
	Gfx::ShaderID			m_theBackFaceShaders[MAX_MESH_COMPONENTS];
	Gfx::Geometry			m_theTextureCubeMeshGeometry;
	Gfx::Geometry			m_theTextureCylinderMeshGeometry;
	Gfx::Geometry			m_theTextureSphereMeshGeometry;

	Gfx::TextureID			m_frameBufferLayers[2];
	Gfx::FrameBufferID		m_colorFrameBuffer;
	PostProcessing::CommonPostProcessing m_postProcessing;

	EditionMode				m_editionMode;
	TextureVisualization	m_textureVisualization;
	int						m_textureId;
};

Tool::MouseState mouseState = { 0, 0, 0, false, false, false, false, false, };
Tool::MouseState mousePreviousState = { 0, 0, 0, false, false, false, false, false, };
Tool::KeyboardState keyboardState = { false, false, false, false, false };
Container::Array<platform::KeyEvent> pendingKeyEvents(128);

static bool HandleMouse(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down)
{
	mouseState.x = x;
	mouseState.y = y;
	mouseState.wheel += wheel;
	mouseState.left = leftDown;
	mouseState.right = rightDown;
	mouseState.middle = middleDown;
	mouseState.x1 = x1Down;
	mouseState.x2 = x2Down;
	return false;
}

static bool HandleKey(platform::KeyCode::Enum key, bool pressed)
{
	platform::KeyEvent event = { key, pressed };
	pendingKeyEvents.add(event);
	return false;
}

void TreatMessages(MeshPlayground* scene)
{
	const Tool::MouseState mouseDiff = Tool::MouseState::MouseDiff(mousePreviousState, mouseState);
	mousePreviousState = mouseState;
	if (mouseDiff.x != 0 || mouseDiff.y != 0 || mouseDiff.wheel != 0)
	{
		scene->OnMouseMove(mouseState, mouseDiff, keyboardState);
	}
	if (mouseDiff.left || mouseDiff.right || mouseDiff.middle || mouseDiff.x1 || mouseDiff.x2)
	{
		scene->OnMouseButton(mouseState, mouseDiff, keyboardState);
	}

	for (int i = 0; i < pendingKeyEvents.size; ++i)
	{
		const platform::KeyEvent& event = pendingKeyEvents[i];
		keyboardState.ReadEvent(event);
		scene->OnKeyboard(event, keyboardState);
	}
	pendingKeyEvents.empty();
}

void UpdateWindowTitle(const platform::Platform& platform, long time)
{
	std::ostringstream oss;
	oss << "Mesh playground - "
		<< time / 1000 << "s";
	std::string title = oss.str();
	platform.SetWindowTitle(title.c_str());
}

void run()
{
	platform::Platform platform("MeshPlayground",
								1024, 768, 0, 0, 1920, 1080, false);
	platform.AddInputHandler(nullptr, HandleKey, HandleMouse);

	Gfx::IGraphicLayer* gfxLayer = new Gfx::OpenGLLayer();
	gfxLayer->CreateRenderingContext();

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
	runtime::FileWatcher fileWatcher;
	runtime::FileWatcher* pFileWatcher = &fileWatcher;
#else // !ENABLE_AUTOMATIC_FILE_RELOAD
#define pFileWatcher NULL
#endif // ENABLE_AUTOMATIC_FILE_RELOAD

#ifdef ENABLE_RUNTIME_COMPILATION
	runtime::RuntimeCompiler runtimeCompiler;
	runtime::RuntimeCompiler* pRuntimeCompiler = &runtimeCompiler;
#else // !ENABLE_RUNTIME_COMPILATION
#define pRuntimeCompiler NULL
#endif // !ENABLE_RUNTIME_COMPILATION

	MeshPlayground scene(gfxLayer, pFileWatcher, pRuntimeCompiler);
	scene.Init(1024, 768);

	const long startTime = platform.GetTime();
	long lastFileUpdateTime = 0;
	while (platform.HandleMessages())
	{
		const long frameStartTime = platform.GetTime();
		const long currentTime = frameStartTime - startTime;

		TreatMessages(&scene);

		if (currentTime - lastFileUpdateTime > 100)
		{
#ifdef ENABLE_TWEAKVAL
			ReloadChangedTweakableValues();
#endif // ENABLE_TWEAKVAL

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
			fileWatcher.Update();
#endif // ENABLE_AUTOMATIC_FILE_RELOAD

#ifdef ENABLE_RUNTIME_COMPILATION
			runtimeCompiler.Update(currentTime - lastFileUpdateTime);
#endif // ENABLE_RUNTIME_COMPILATION
			lastFileUpdateTime = currentTime;
		}
		scene.Update(currentTime);
		scene.Draw(currentTime);

		gfxLayer->EndFrame();
		platform.SwapBuffers();

		UpdateWindowTitle(platform, currentTime);
	}

	gfxLayer->DestroyRenderingContext();
	delete gfxLayer;
}

int main()
{
	LOG_INFO("Starting MeshPlayground");

#if _HAS_EXCEPTIONS
	try
	{
		run();
	}
	catch (std::exception* e)
	{
		LOG_FATAL(e->what());
		return 1;
	}
#else
	run();
#endif // _HAS_EXCEPTIONS

	LOG_INFO("Ending MeshPlayground");
	return 0;
}

#if _WIN32
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	return main();
}
#endif
