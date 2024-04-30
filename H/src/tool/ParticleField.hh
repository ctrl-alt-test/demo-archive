#ifndef PARTICLE_FIELD_HH
#define PARTICLE_FIELD_HH

#include "engine/algebra/Vector3.hh"
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"

namespace Gfx
{
	class IGraphicLayer;
	struct DrawArea;
	struct RasterTests;
}

namespace Core
{
	struct Camera;
	struct Light;
}

namespace Tool
{
	class ShaderLoader;
	class TextureLoader;
	struct FixedLightRig;
	struct ParticipatingMedium;

	struct ParticleField
	{
	public:
		void Init(Gfx::IGraphicLayer* gfxLayer,
				  int numParticules,
				  long updateDT,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader,
				  Tool::TextureLoader* textureLoader);
		void SetVolume(float x, float y, float z,
					   float width, float height, float length);
		void Update(long currentTime);
		void Draw(long currentTime,
				  const Gfx::DrawArea& drawArea,
				  const Core::Camera& camera,
				  const Tool::FixedLightRig& lightRig,
				  const ParticipatingMedium& medium);

	public:
		float					particleSize;
		float					forwardScatteringStrength;
		float					verticalSpeed;
		float					submersibleHeight; // Last minute hack.
		Gfx::ShadingParameters	computeProgram;
		Tool::CommonMaterial	renderMaterial;

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;
		Gfx::TextureID*			m_randomTexture;
		Gfx::ShadingParameters	m_shading;

		Gfx::Geometry			m_computeGeometry;
		Gfx::Geometry			m_renderGeometry;

		Gfx::TextureID			m_pingFBTextures[1];
		Gfx::TextureID			m_pongFBTextures[1];
		Gfx::FrameBufferID		m_pingFB;
		Gfx::FrameBufferID		m_pongFB;

		bool					m_isInitialized;
		bool					m_ping;
		long					m_updateDT;
		long					m_nextUpdateTime;

		Algebra::vector3f		m_position;
		Algebra::vector3f		m_size;
	};
}

#endif // PARTICLE_FIELD_HH
