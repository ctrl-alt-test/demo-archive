#pragma once

#include "LoadingBar.hh"
#include "engine/core/Camera.hh"
#include "engine/timeline/Timeline.hh"
#include "postprocessing/CommonPostProcessing.hh"
#include "tool/DebugControls.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/MeshLoader.hh"
#include "tool/ProjectLoader.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tool/VolumetricLighting.hh"

class BoatScene;
class CityScene;
class DescentScene;
class EmergenceScene;
class InsideTempleScene;
class RuinsScene;
class ViaductScene;

namespace Core
{
	class Settings;
}

namespace runtime
{
	class FileWatcher;
	class RuntimeCompiler;
}

namespace Tool
{
	class RenderList;
}

class H
{
public:
	H(Gfx::IGraphicLayer* gfxLayer,
	  Tool::DebugControls* debugControls,
	  runtime::FileWatcher* fileWatcher,
	  runtime::RuntimeCompiler* runtimeCompiler):
		m_gfxLayer(gfxLayer),
#if DEBUG
		m_debugControls(debugControls),
#endif // DEBUG
		m_meshLoader(gfxLayer, runtimeCompiler),
		m_projectLoader(fileWatcher),
		m_shaderLoader(gfxLayer, fileWatcher),
		m_textureLoader(gfxLayer, runtimeCompiler),
		m_fileWatcher(fileWatcher),
		m_volumetricLighting(gfxLayer),
		m_postProcessing(gfxLayer)
	{
	}

	void Init(const Core::Settings& settings);
	void CreateShots();
	void Draw(long currentTime);

private:
	Gfx::IGraphicLayer*		m_gfxLayer;
	Tool::RenderList*		m_renderList;
#if DEBUG
	Tool::DebugControls*	m_debugControls;
#endif // DEBUG
	Tool::MeshLoader		m_meshLoader;
	Tool::ProjectLoader		m_projectLoader;
	Tool::ShaderLoader		m_shaderLoader;
	Tool::TextureLoader		m_textureLoader;
	int						m_renderWidth;
	int						m_renderHeight;
	Timeline::Timeline		m_timeline;
	Core::CameraDescription	m_camera;
	const Timeline::Shot*	m_shot;
	runtime::FileWatcher*	m_fileWatcher;

	Gfx::TextureID			m_frameBufferLayers[2];
	Gfx::FrameBufferID		m_colorFrameBuffer;

	// Reflection & refraction color buffers.
	Gfx::TextureID			m_reflectionTextures[2];
	Gfx::TextureID			m_refractionTextures[2];
	Gfx::FrameBufferID		m_reflectionFB;
	Gfx::FrameBufferID		m_refractionFB;

	Tool::FixedLightToolBox	m_lightToolBox;
	Tool::VolumetricLighting m_volumetricLighting;

	PostProcessing::CommonPostProcessing m_postProcessing;

	BoatScene*				m_boatScene;
	CityScene*				m_cityScene;
	DescentScene*			m_descentScene;
	EmergenceScene*			m_emergenceScene;
	InsideTempleScene*		m_insideTempleScene;
	RuinsScene*				m_ruinsScene;
	ViaductScene*			m_viaductScene;
};
