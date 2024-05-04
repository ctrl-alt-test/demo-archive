#include "H.hh"

#include "BoatScene.hh"
#include "CityScene.hh"
#include "DescentScene.hh"
#include "EmergenceScene.hh"
#include "InsideTempleScene.hh"
#include "RuinsScene.hh"
#include "ViaductScene.hh"
#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/Settings.hh"
#include "engine/runtime/FileWatcher.hh"
#include "engine/timeline/Anim.hxx"
#include "engine/timeline/Timeline.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/CameraLoader.hh"
#include "tool/Quad.hh"
#include "tool/RenderList.hh"
#include "tool/TextureHelper.hh"
#include "tweakval/tweakval.h"
#include <cassert>

using namespace Algebra;

#define SHOTS_FILE "assets/shots.json"

#if DEBUG
void H::CreateShots()
{
	ParseCameraFile(SHOTS_FILE, m_timeline, m_debugControls->allScenes);
}
#else
static quaternion* unpackQuaternions(compressedQuaternion* cquat, int n)
{
	quaternion* quat = (quaternion*)malloc(n * sizeof(quaternion));
	
	for (int i = 0; i < n; i++) {
		quat[i] = cquat[i].q();
	}
	return quat;
}

# include "exported_camera.hh"
#endif

void H::Init(const Core::Settings& settings)
{
	m_renderWidth = settings.renderWidth;
	m_renderHeight = settings.renderHeight;
	m_renderList = new Tool::RenderList();
	m_renderList->Init(m_gfxLayer);

#if VAR_EDIT
	m_projectLoader.LoadAndMonitorProject("assets/H.project");
#else // !VAR_EDIT
	m_projectLoader.LoadProject();
#endif // !VAR_EDIT

#if !SHADER_EDIT
	// Compile the shaders first, so we have a shader ready for the
	// loading screen.
	for (int i = 0; m_shaderLoader.LoadShadersIncrementally(i); ++i)
	{
		LoadingBar::update(&m_shaderLoader);
	}
#endif // !SHADER_EDIT
#if !MESH_EDIT
	for (int i = 0; m_meshLoader.LoadMeshesIncrementally(i); ++i)
	{
		LoadingBar::update();
	}
#endif // !MESH_EDIT
#if !TEXTURE_EDIT
	for (int i = 0; m_textureLoader.LoadTexturesIncrementally(i, settings); ++i)
	{
		LoadingBar::update();
	}
#endif // !TEXTURE_EDIT

	m_boatScene = new BoatScene();
	m_cityScene = new CityScene();
	m_descentScene = new DescentScene();
	m_emergenceScene = new EmergenceScene();
	m_insideTempleScene = new InsideTempleScene();
	m_ruinsScene = new RuinsScene();
	m_viaductScene = new ViaductScene();

#if DEBUG
	m_debugControls->allScenes["m_boatScene"] = m_boatScene;
	m_debugControls->allScenes["m_cityScene"] = m_cityScene;
	m_debugControls->allScenes["m_descentScene"] = m_descentScene;
	m_debugControls->allScenes["m_emergenceScene"] = m_emergenceScene;
	m_debugControls->allScenes["m_insideTempleScene"] = m_insideTempleScene;
	m_debugControls->allScenes["m_ruinsScene"] = m_ruinsScene;
	m_debugControls->allScenes["m_viaductScene"] = m_viaductScene;
#endif // DEBUG

	CreateShots();
#if DEBUG
	m_fileWatcher->OnFileUpdated(SHOTS_FILE, std::bind(&H::CreateShots, this), 56157482);
#endif // DEBUG
	LoadingBar::update(&m_shaderLoader);

	Gfx::Geometry quad = Tool::LoadQuadMesh(m_gfxLayer);
	Tool::TextureHelper textureHelper(m_gfxLayer);
	m_frameBufferLayers[0] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::R11G11B10f);
	m_frameBufferLayers[1] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::Depth);
	m_colorFrameBuffer = m_gfxLayer->CreateFrameBuffer(m_frameBufferLayers, ARRAY_LEN(m_frameBufferLayers));

	m_reflectionTextures[0] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::R11G11B10f);
	m_reflectionTextures[1] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::Depth);
	m_reflectionFB = m_gfxLayer->CreateFrameBuffer(m_reflectionTextures, ARRAY_LEN(m_reflectionTextures));

	m_refractionTextures[0] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::R11G11B10f);
	m_refractionTextures[1] = textureHelper.CreateRenderTexture(m_renderWidth, m_renderHeight, Gfx::TextureFormat::Depth);
	m_refractionFB = m_gfxLayer->CreateFrameBuffer(m_refractionTextures, ARRAY_LEN(m_refractionTextures));

	m_postProcessing.Init(m_gfxLayer, quad, &m_shaderLoader, &m_textureLoader,
		settings.renderWidth, settings.renderHeight,
		settings.windowWidth, settings.windowHeight);

	m_lightToolBox.Init(m_gfxLayer, &m_shaderLoader, quad, settings.TextureSizeAtLeast(512));
	LoadingBar::update();
	int marchingSamples = settings.TextureSizeAbout(16);
#if DEBUG
	marchingSamples = msys_max(6, marchingSamples);
#endif // DEBUG
	m_volumetricLighting.Init(m_renderWidth, m_renderHeight, marchingSamples, quad, &m_shaderLoader, &m_textureLoader, &textureHelper);
	LoadingBar::update();

	m_boatScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1],
		m_reflectionTextures, m_refractionTextures, m_reflectionFB, m_refractionFB, &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_emergenceScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1],
		m_reflectionTextures, m_refractionTextures, m_reflectionFB, m_refractionFB, &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_cityScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1], &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_descentScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1], &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_insideTempleScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1], &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_ruinsScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1], &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
	m_viaductScene->Init(m_gfxLayer, m_renderList, settings, &m_meshLoader, &m_shaderLoader, &m_textureLoader, m_frameBufferLayers[1], &m_lightToolBox, &m_volumetricLighting, quad);
	LoadingBar::update();
}

vector3f DecodeRGBM(const vector4f& rgbm)
{
	const vector3f result = {
		rgbm.x * rgbm.w,
		rgbm.y * rgbm.w,
		rgbm.z * rgbm.w
	};
	return result;
}

void H::Draw(long youtubeTime)
{
	m_shot = &m_timeline.GetShot(youtubeTime);

#if DEBUG
	m_debugControls->defaultShot = m_shot;
	m_camera = m_debugControls->GetCamera();
	int storyTime = m_debugControls->GetStoryTime();
	Timeline::IScene* scene = m_debugControls->GetScene();
#else // !DEBUG
	m_camera = m_shot->Get(youtubeTime - m_shot->youtubeTime);
	int storyTime = m_shot->GetStoryTime(youtubeTime);
	Timeline::IScene* scene = m_shot->scene;
#endif // !DEBUG
	assert(scene != NULL);

	const Gfx::DrawArea mainDrawArea = { m_colorFrameBuffer, { 0, 0, m_renderWidth, m_renderHeight } };
	// Since we're rendering to a buffer and just doing a blit at the
	// end, clearing shouldn't be necessary.
	// const Gfx::DrawArea directToScreen = { { -1 }, { 0, 0, m_renderWidth, m_renderHeight } };
	// m_gfxLayer->Clear(directToScreen.frameBuffer, 0.f, 0.f, 0.f, true);
	scene->Draw(storyTime, m_camera, mainDrawArea);

	// Post-processing.
	const vector3f horizontalStreakPower = DecodeRGBM(VAR(post_horizontalStreakColor)->get4());
	const vector3f verticalStreakPower = DecodeRGBM(VAR(post_verticalStreakColor)->get4());
	vector3f bloomCombine = DecodeRGBM(VAR(post_bloomCombineColor)->get4(float(youtubeTime)));
	vector3f streakCombine = DecodeRGBM(VAR(post_streakCombineColor)->get4(float(youtubeTime)));

	const float gainIntensityInfo = VAR(post_gainIntensity)->get1(float(youtubeTime));
	const vector3f lift = vector3f::zero;
	const vector3f gamma = { _TV(1.0f), _TV(1.0f), _TV(1.0f) };
	const vector3f gain = { gainIntensityInfo, gainIntensityInfo, gainIntensityInfo };

#if DEBUG
	if (!m_debugControls->enableBloom)
	{
		bloomCombine = vector3f::zero;
	}
	if (!m_debugControls->enableLightStreak)
	{
		streakCombine = vector3f::zero;
	}
#endif // DEBUG

	m_postProcessing.Apply(youtubeTime,
						   m_frameBufferLayers[0],
						   bloomCombine,
						   horizontalStreakPower,
						   verticalStreakPower,
						   streakCombine,
						   lift, gamma, gain,
						   _TV(10.0f),
						   _TV(1.2f),
						   _TV(1.0f));
}
