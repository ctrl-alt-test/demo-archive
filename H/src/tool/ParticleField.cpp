#include "ParticleField.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "gfx/Uniform.hxx"
#include "textures/RandomTexture.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include "tool/TextureLoader.hh"
#include "tweakval/tweakval.h"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace Tool;

void ParticleField::Init(IGraphicLayer* gfxLayer,
						 int numParticules,
						 long updateDT,
						 const Geometry& quad,
						 Tool::ShaderLoader* shaderLoader,
						 TextureLoader* textureLoader)
{
	m_gfxLayer = gfxLayer;

	m_bufferWidth =  512;
	m_bufferHeight = (numParticules + m_bufferWidth - 1) / m_bufferWidth;
	m_computeGeometry = quad;
	m_renderGeometry = quad;
	m_renderGeometry.numberOfInstances = numParticules;

	m_updateDT = updateDT;
	m_nextUpdateTime = 0;
	m_isInitialized = false;
	m_ping = false;

	verticalSpeed = 0.001f;

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);
	LOAD_SHADER(*shaderLoader, &computeProgram.shader, particleFieldCompute, "assets/shaders/screenspace/blitShader.vs", "assets/shaders/compute/seaParticles.fs");
	LOAD_SHADER(*shaderLoader, &renderMaterial.shader, particleFieldRender, "assets/shaders/particle.vs", "assets/shaders/transparentParticle.fs");

	// Frame buffers
	Tool::TextureHelper textureHelper(m_gfxLayer);
	m_pingFBTextures[0] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::RGBA32f);
	m_pingFB = m_gfxLayer->CreateFrameBuffer(m_pingFBTextures, ARRAY_LEN(m_pingFBTextures));

	m_pongFBTextures[0] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::RGBA32f);
	m_pongFB = m_gfxLayer->CreateFrameBuffer(m_pongFBTextures, ARRAY_LEN(m_pongFBTextures));
}

void ParticleField::SetVolume(float x, float y, float z,
							  float width, float height, float length)
{
#if DEBUG
	const Algebra::vector3f newPosition = { x, y, z };
	const Algebra::vector3f newSize = { width, height, length };
	if (m_position != newPosition || m_size != newSize)
	{
		m_isInitialized = false;
	}
#endif
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_size.x = width;
	m_size.y = height;
	m_size.z = length;
}

void ParticleField::Update(long currentTime)
{
	// When rewinding time in debug
	if (currentTime + 2 * m_updateDT < m_nextUpdateTime)
	{
		m_nextUpdateTime = currentTime;
		return;
	}

	computeProgram.uniforms.empty();
	Tool::ShaderHelper::ExposeFullScreenBlitTransform(computeProgram.uniforms);
	Tool::ShaderHelper::ExposeResolutionInfo(computeProgram.uniforms, m_bufferWidth, m_bufferHeight);
	Tool::ShaderHelper::ExposeTimeInfo(computeProgram.uniforms, 1000);
	computeProgram.uniforms.add(Uniform::Float1(U_INIT, (m_isInitialized ? 0.f : 1.f)));
	computeProgram.uniforms.add(Uniform::Float1(U_AMPLITUDE, _TV(0.5f)));
	computeProgram.uniforms.add(Uniform::Float1(U_HEIGHT, submersibleHeight));
	computeProgram.uniforms.add(Uniform::Float2(U_SPEED, _TV(0.0015f), verticalSpeed));
	computeProgram.uniforms.add(Uniform::Float3(U_VOLUMESIZE, m_size.x, m_size.y, m_size.z));
	computeProgram.uniforms.add(Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]));
	computeProgram.uniforms.add(Uniform::Sampler1(U_TEXTURE0, TextureID::InvalidID));

	assert(strcmp(computeProgram.uniforms[5].name, U_TIME) == 0);
	assert(strcmp(computeProgram.uniforms[12].name, U_TEXTURE0) == 0);

	long calcDate = m_nextUpdateTime;
	if (calcDate + 256 < currentTime)
	{
		// If there is too much backlog, just skip to currentTime.
		calcDate = currentTime;
	}

	while (calcDate <= currentTime + m_updateDT)
	{
		m_ping = !m_ping;

		computeProgram.uniforms[5].fValue[0] = calcDate / 1000.f;
		computeProgram.uniforms[12].id = (m_ping ? m_pingFBTextures[0] : m_pongFBTextures[0]);
		DrawArea target = { (m_ping ? m_pongFB : m_pingFB), { 0, 0, m_bufferWidth, m_bufferHeight } };
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_computeGeometry, computeProgram);
		m_isInitialized = true;

		calcDate += m_updateDT;
	}
	m_nextUpdateTime = calcDate;
}

void ParticleField::Draw(long currentTime,
						 const DrawArea& drawArea,
						 const Core::Camera& camera,
						 const Tool::FixedLightRig& lightRig,
						 const ParticipatingMedium& medium)
{
	const Algebra::matrix4 model = Algebra::matrix4::translation(m_position);
	renderMaterial.SetShaderParameters(m_shading, model, currentTime, camera, lightRig, medium, false);
	m_shading.uniforms.add(Uniform::Float1(U_PARTICLESIZE, particleSize));
	m_shading.uniforms.add(Uniform::Float1(U_FORWARDSCATTERINGSTRENGTH, forwardScatteringStrength));
	m_shading.uniforms.add(Uniform::Int2(U_COMPUTEBUFFERSIZE, m_bufferWidth, m_bufferHeight));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, (m_ping ? m_pingFBTextures[0] : m_pongFBTextures[0])));
	m_shading.blendingMode = BlendingMode::Translucent;

	RasterTests depthReadOnly(FaceCulling::None, DepthFunction::Less, false);
	m_gfxLayer->Draw(drawArea, depthReadOnly, m_renderGeometry, m_shading);
}
