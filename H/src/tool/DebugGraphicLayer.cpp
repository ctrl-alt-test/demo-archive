#include "DebugGraphicLayer.hh"

#include "DebugControls.hh"
#include "engine/container/Array.hxx"
#include "engine/core/FileIO.hh"
#include "engine/core/StringUtils.hh"
#include "gfx/DrawArea.hh"
#include "gfx/RasterTests.hh"
#include "gfx/Uniform.hxx"
#include "tool/Quad.hh"
#include <cassert>
#include <cstring>

using namespace Tool;
using namespace Gfx;

DebugGraphicLayer::DebugGraphicLayer(IGraphicLayer& gfxLayer,
									 const DebugControls* debugControls):
	m_gfxLayer(gfxLayer),
	m_debugControls(debugControls),
	m_shading(),
	m_customDrawCallShading1(),
	m_customDrawCallShading2()
{
}

void DebugGraphicLayer::CreateRenderingContext()
{
	m_gfxLayer.CreateRenderingContext();
	InitBlitDisplay();
}

void DebugGraphicLayer::DestroyRenderingContext()
{
	m_gfxLayer.DestroyRenderingContext();
}

VertexBufferID DebugGraphicLayer::CreateVertexBuffer()
{
	VertexBufferInfo info;
	info.vertexBuffer = m_gfxLayer.CreateVertexBuffer();
	info.stride = 0;
	info.vertexDataSize = 0;
	info.indexDataSize = 0;
	m_vertexBuffersInfo.push_back(info);

	return info.vertexBuffer;
}

const VertexBufferID& DebugGraphicLayer::LoadVertexBuffer(const VertexBufferID& id,
														  const VertexAttribute* vertexAttributes,
														  int numberOfAttributes, int stride,
														  int vertexDataSize, void* vertexData,
														  int indexDataSize, void* indexData,
														  VertexIndexType::Enum indexType)
{
	for (size_t i = 0; i < m_vertexBuffersInfo.size(); ++i)
	{
		VertexBufferInfo& info = m_vertexBuffersInfo[i];
		if (info.vertexBuffer.index == id.index)
		{
			m_gfxLayer.LoadVertexBuffer(id,
										vertexAttributes,
										numberOfAttributes, stride,
										vertexDataSize, vertexData,
										indexDataSize, indexData,
										indexType);
			info.stride = stride;
			info.vertexDataSize = vertexDataSize;
			info.indexDataSize = indexDataSize;

			return id;
		}
	}

	// Trying to load a vertex buffer without calling CreateVertexBuffer() first?
	assert(false);

	return id;
}

void DebugGraphicLayer::DestroyVertexBuffer(const VertexBufferID& id)
{
	m_gfxLayer.DestroyVertexBuffer(id);
	for (size_t i = 0; i < m_vertexBuffersInfo.size(); ++i)
	{
		const VertexBufferInfo& info = m_vertexBuffersInfo[i];
		if (info.vertexBuffer.index == id.index)
		{
			m_vertexBuffersInfo.erase(m_vertexBuffersInfo.begin() + i);
			break;
		}
	}
}

TextureID DebugGraphicLayer::CreateTexture()
{
	TextureInfo info;
	info.texture = m_gfxLayer.CreateTexture();
	info.width = 0;
	info.height = 0;
	m_texturesInfo.push_back(info);

	return info.texture;
}

const TextureID& DebugGraphicLayer::LoadTexture(const TextureID& id,
												int width, int height,
												TextureType::Enum textureType,
												TextureFormat::Enum textureFormat,
												int side, int lodLevel,
												void* data,
												const TextureSampling& textureSampling)
{
	for (size_t i = 0; i < m_texturesInfo.size(); ++i)
	{
		TextureInfo& info = m_texturesInfo[i];
		if (info.texture.index == id.index)
		{
			m_gfxLayer.LoadTexture(id,
								   width, height,
								   textureType,
								   textureFormat,
								   side, lodLevel,
								   data,
								   textureSampling);
			info.width = width;
			info.height = height;
			info.type = textureType,
			info.format = textureFormat;

			return id;
		}
	}

	// Trying to load a texture without calling CreateTexture() first?
	assert(false);

	return id;
}

void DebugGraphicLayer::DestroyTexture(const TextureID& id)
{
	m_gfxLayer.DestroyTexture(id);
	for (size_t i = 0; i < m_texturesInfo.size(); ++i)
	{
		const TextureInfo& info = m_texturesInfo[i];
		if (info.texture.index == id.index)
		{
			m_texturesInfo.erase(m_texturesInfo.begin() + i);
			break;
		}
	}
}

ShaderID DebugGraphicLayer::CreateShader()
{
	ShaderInfo info;
	info.shader = m_gfxLayer.CreateShader();

	info.whiteShader = m_gfxLayer.CreateShader();
	info.unlitWhiteShader = m_gfxLayer.CreateShader();
	info.highlightShader = m_gfxLayer.CreateShader();
	info.showTexCoordShader = m_gfxLayer.CreateShader();
	m_shadersInfo.push_back(info);

	return info.shader;
}

const ShaderID& DebugGraphicLayer::LoadShader(const ShaderID& id,
											  const char* vertexShaderSource,
											  const char* vertexShaderSourceInfo,
											  const char* fragmentShaderSource,
											  const char* fragmentShaderSourceInfo)
{
	for (size_t i = 0; i < m_shadersInfo.size(); ++i)
	{
		ShaderInfo& info = m_shadersInfo[i];
		if (info.shader.index == id.index)
		{
			m_gfxLayer.LoadShader(id,
								  vertexShaderSource,
								  vertexShaderSourceInfo,
								  fragmentShaderSource,
								  fragmentShaderSourceInfo);

			const char* screenSpaceFolder1 = "assets/shaders/screenspace/";
			const char* screenSpaceFolder2 = "assets\\shaders\\screenspace\\";
			bool isScreenSpaceShader =
				(std::string(vertexShaderSourceInfo).find(screenSpaceFolder1) != std::string::npos) ||
				(std::string(vertexShaderSourceInfo).find(screenSpaceFolder2) != std::string::npos);

			if (!isScreenSpaceShader)
			{
				// FIXME: detect if the debug shader didn't compile;
				// not sure what we can do then except fail silently.

				// Creation of alternate debug shaders
				std::vector<std::string> files;
				std::string preprocessedFile;

				std::string whiteFsContent = Core::PreprocessFile("assets/shaders/debug/whiteDiffuseShader.fs", &files, &preprocessedFile);
				m_gfxLayer.LoadShader(info.whiteShader, vertexShaderSource, vertexShaderSourceInfo, whiteFsContent.c_str(), preprocessedFile.c_str());

				std::string unlitWhiteFsContent = Core::PreprocessFile("assets/shaders/debug/unlitWhiteShader.fs", &files, &preprocessedFile);
				m_gfxLayer.LoadShader(info.unlitWhiteShader, vertexShaderSource, vertexShaderSourceInfo, unlitWhiteFsContent.c_str(), preprocessedFile.c_str());

				std::string highlightFsContent = Core::PreprocessFile("assets/shaders/debug/singleColorShader.fs", &files, &preprocessedFile);
				m_gfxLayer.LoadShader(info.highlightShader, vertexShaderSource, vertexShaderSourceInfo, highlightFsContent.c_str(), preprocessedFile.c_str());

				std::string showTexCoordFsContent = Core::PreprocessFile("assets/shaders/debug/showTexCoord.fs", &files, &preprocessedFile);
				m_gfxLayer.LoadShader(info.showTexCoordShader, vertexShaderSource, vertexShaderSourceInfo, showTexCoordFsContent.c_str(), preprocessedFile.c_str());
			}
			else
			{
				info.whiteShader = ShaderID::InvalidID;
				info.unlitWhiteShader = ShaderID::InvalidID;
				info.highlightShader = ShaderID::InvalidID;
				info.showTexCoordShader = ShaderID::InvalidID;
			}

			m_shadersInfo.push_back(info);

			return id;
		}
	}

	assert(false);

	return id;
}

void DebugGraphicLayer::DestroyShader(const ShaderID& id)
{
	m_gfxLayer.DestroyShader(id);

	for (size_t i = 0; i < m_shadersInfo.size(); ++i)
	{
		const ShaderInfo& info = m_shadersInfo[i];
		if (info.shader.index == id.index)
		{
			m_gfxLayer.DestroyShader(info.whiteShader);
			m_gfxLayer.DestroyShader(info.unlitWhiteShader);
			m_gfxLayer.DestroyShader(info.highlightShader);
			m_gfxLayer.DestroyShader(info.showTexCoordShader);

			m_shadersInfo.erase(m_shadersInfo.begin() + i);
			break;
		}
	}
}

FrameBufferID DebugGraphicLayer::CreateFrameBuffer(const TextureID* textures, int numberOfTextures)
{
	FrameBufferInfo info;
	info.frameBuffer = m_gfxLayer.CreateFrameBuffer(textures, numberOfTextures);
	for (int i = 0; i < numberOfTextures; ++i)
	{
		info.textures.push_back(textures[i]);
	}
	m_frameBuffersInfo.push_back(info);

	return info.frameBuffer;
}

void DebugGraphicLayer::DestroyFrameBuffer(const FrameBufferID& id)
{
	m_gfxLayer.DestroyFrameBuffer(id);
	for (size_t i = 0; i < m_frameBuffersInfo.size(); ++i)
	{
		const FrameBufferInfo& info = m_frameBuffersInfo[i];
		if (info.frameBuffer.index == id.index)
		{
			m_frameBuffersInfo.erase(m_frameBuffersInfo.begin() + i);
			break;
		}
	}
}

void DebugGraphicLayer::Clear(const FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth)
{
	m_gfxLayer.Clear(frameBuffer, r, g, b, clearDepth);
}

/// <summary>
/// Guesses from the material parameters and indices whether this is a
/// fullscreen quad or not.
/// </summary>
static bool IsFullScreenBlit(const ShadingParameters& shadingParameters, int numberOfIndices)
{
	if (numberOfIndices > 6)
	{
		return false;
	}

	const float* projection = NULL;
	for (int i = 0; i < shadingParameters.uniforms.size; ++i)
	{
		// Hacky string compare to retrieve the projection matrix used.
		if (strcmp(shadingParameters.uniforms[i].name, U_PROJECTIONMATRIX) == 0)
		{
			projection = shadingParameters.uniforms[i].fValue;
			break;
		}
	}
	assert(projection != NULL);

	Algebra::matrix4 fullScreenBlit = Algebra::matrix4::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	for (int i = 0; i < 16; ++i)
	{
		if (projection[i] != fullScreenBlit.m[i])
		{
			return false;
		}
	}
	return true;
}

/// <summary>
/// Guesses from the material parameters whether light is used or not.
/// </summary>
static bool IsLit(const ShadingParameters& shadingParameters)
{
	for (int i = 0; i < shadingParameters.uniforms.size; ++i)
	{
		if (strcmp(shadingParameters.uniforms[i].name, Tool::lightColorParamNames[0]) == 0)
		{
			return true;
		}
	}
	return false;
}

void DebugGraphicLayer::Draw(const DrawArea& drawArea,
							 const RasterTests& rasterTests,
							 const Geometry& geometry,
							 const ShadingParameters& shadingParameters)
{
	++m_drawCallsCount;

	// Drop draw calls above m_drawCallsCount, to show all the steps
	// of how the frame is constructed.
	// Based on this idea:
	// http://blogs.unity3d.com/2014/07/29/frame-debugger-in-unity-5-0/
	if (m_debugControls->stopAtDrawCall != 0 &&
		m_drawCallsCount > m_debugControls->stopAtDrawCall)
	{
		return;
	}

	RasterTests customDepthTest = rasterTests;
#if GFX_ENABLE_FACE_CULLING
	if (!m_debugControls->enableFaceCulling)
	{
		customDepthTest.faceCulling = FaceCulling::None;
	}
#endif // GFX_ENABLE_FACE_CULLING

	// For the actual drawcall except for full screen blits, we create
	// a custom material that is a copy of an input material, with
	// potentially different parameters, depending on the active debug
	// options.
	m_customDrawCallShading1 = shadingParameters;
	if (!IsFullScreenBlit(shadingParameters, geometry.numberOfIndices))
	{
		// Debug shaders.
		if (m_debugControls->shadingMode != DebugControls::Standard)
		{
			size_t i = 0;
			while (i < m_shadersInfo.size())
			{
				if (m_shadersInfo[i].shader.index == shadingParameters.shader.index)
				{
					break;
				}
				++i;
			}
			assert(i < m_shadersInfo.size());

			switch (m_debugControls->shadingMode)
			{
			case DebugControls::Highlight:
				m_customDrawCallShading1.shader = m_shadersInfo[i].highlightShader;
				m_customDrawCallShading1.uniforms.add(Uniform::Float4("color", 1.f, 1.f, 0.f, 1.f));
				break;
			case DebugControls::WhiteMaterial:
				m_customDrawCallShading1.shader = (IsLit(shadingParameters) ?
												   m_shadersInfo[i].whiteShader :
												   m_shadersInfo[i].unlitWhiteShader);
				break;
			case DebugControls::ShowTexCoord:
				m_customDrawCallShading1.shader = m_shadersInfo[i].showTexCoordShader;
				break;
			default:
				break;
			}
		}

		// Line mode.
		if (m_debugControls->wireFrame)
		{
			m_customDrawCallShading1.polygonMode = PolygonMode::Lines;
		}

		// Line mode and highlight shader for current drawcall.
		if (m_debugControls->stopAtDrawCall != 0 &&
			m_drawCallsCount == m_debugControls->stopAtDrawCall)
		{
			m_customDrawCallShading2 = m_customDrawCallShading1;
			m_customDrawCallShading2.polygonMode = PolygonMode::Lines;

			for (size_t i = 0; i < m_shadersInfo.size(); ++i)
			{
				if (m_shadersInfo[i].shader.index == shadingParameters.shader.index)
				{
					m_customDrawCallShading2.shader = m_shadersInfo[i].highlightShader;
					break;
				}
			}

			// Draw all polygons in red. After overdrawing, only the polygons
			// that didn't pass the depth test will remain visible.
			RasterTests noDepthTest = customDepthTest;
			noDepthTest.enableClipDistance = false;
#if GFX_ENABLE_DEPTH_TESTING
			noDepthTest.depthTest = DepthFunction::Always;
			noDepthTest.depthWrite = false;
#endif // GFX_ENABLE_DEPTH_TESTING
			m_customDrawCallShading2.uniforms.add(Uniform::Float4("color", 1.f, 0.f, 0.f, 1.f));
			m_gfxLayer.Draw(drawArea,
							noDepthTest,
							geometry,
							m_customDrawCallShading2);

			// Draw polygons in green.
			m_customDrawCallShading2.uniforms.last() = Uniform::Float4("color", 0.f, 1.f, 0.f, 1.f);
			m_gfxLayer.Draw(drawArea,
							customDepthTest,
							geometry,
							m_customDrawCallShading2);
		}
	}
	m_gfxLayer.Draw(drawArea,
					customDepthTest,
					geometry,
					m_customDrawCallShading1);

	// If some draw calls were dropped, make sure the result is shown
	// on screen.
	if (m_debugControls->stopAtDrawCall != 0 &&
		m_drawCallsCount == m_debugControls->stopAtDrawCall)
	{
		DisplayFrameBuffer(drawArea.frameBuffer);
	}
}

void DebugGraphicLayer::EndFrame()
{
	m_gfxLayer.EndFrame();
	ResetDrawCallsCount();
}

void DebugGraphicLayer::InitBlitDisplay()
{
	m_quadGeometry = LoadQuadMesh(&m_gfxLayer);

	std::vector<std::string> files;
	std::string preprocessedVSFile;
	std::string preprocessedFSFile;
	const std::string vsContent = Core::PreprocessFile("assets/shaders/screenspace/blitShader.vs", &files, &preprocessedVSFile);
	const std::string fsContent = Core::PreprocessFile("assets/shaders/screenspace/blitShader.fs", &files, &preprocessedFSFile);
	m_colorBufferShader = m_gfxLayer.LoadShader(
		m_gfxLayer.CreateShader(),
		vsContent.c_str(), preprocessedVSFile.c_str(),
		fsContent.c_str(), preprocessedFSFile.c_str());

	const std::string fsContentDepth = Core::PreprocessFile("assets/shaders/debug/showDepth.fs", &files, &preprocessedFSFile);
	m_depthBufferShader = m_gfxLayer.LoadShader(
		m_gfxLayer.CreateShader(),
		vsContent.c_str(), preprocessedVSFile.c_str(),
		fsContentDepth.c_str(), preprocessedFSFile.c_str());
}

void DebugGraphicLayer::DisplayFrameBuffer(const FrameBufferID& frameBuffer)
{
	if (frameBuffer.index == -1)
	{
		// It's already going to the screen.
		return;
	}

	// Find the frame buffer information.
	TextureID inputImage = { -1 };
	for (size_t i = 0; i < m_frameBuffersInfo.size(); ++i)
	{
		if (m_frameBuffersInfo[i].frameBuffer.index == frameBuffer.index)
		{
			inputImage = m_frameBuffersInfo[i].textures[0];
			break;
		}
	}

	// Find the frame buffer texture information.
	int width = 500;
	int height = 500;
	TextureFormat::Enum format = TextureFormat::RGBA8;
	for (size_t i = 0; i < m_texturesInfo.size(); ++i)
	{
		if (m_texturesInfo[i].texture.index == inputImage.index)
		{
			width = m_texturesInfo[i].width;
			height = m_texturesInfo[i].height;
			format = m_texturesInfo[i].format;
			break;
		}
	}

	// Display a quad textured with the frame buffer texture.
	DrawArea directToScreen = { { -1 }, { 0, 0, width, height } };

	m_gfxLayer.Clear(directToScreen.frameBuffer, 0.25f, 0.25f, 0.25f, false);

	m_shading.uniforms.empty();
	ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	m_shading.uniforms.add(Uniform::Float2(U_INVRESOLUTION, 1.0f / width, 1.0f / height));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, inputImage));
	m_shading.shader = (format == TextureFormat::Depth ? m_depthBufferShader : m_colorBufferShader);

	m_gfxLayer.Draw(directToScreen, RasterTests::TwoSidedNoDepthTest, m_quadGeometry, m_shading);
}
