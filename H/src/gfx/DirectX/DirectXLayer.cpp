#ifdef _WIN32

#include "DirectXLayer.hh"
#include "engine/core/Debug.hh" // FIXME: remove dependency, ideally Gfx should not have dependency over Engine
#include "gfx/Geometry.hh"
#include "gfx/ResourceID.hh"
#include <cassert>
#if DEBUG
#include <sstream>
#endif

#if defined(GFX_MULTI_API) || defined(GFX_DIRECTX_ONLY)

using namespace Gfx;

#define VERTEX_SIZE (8 * sizeof(float))
#define PRIMITIVE_SIZE (4 * VERTEX_SIZE)

#if DEBUG
#define ENABLE_DIRECTX_ERROR_CHECK
#endif // DEBUG

#ifdef ENABLE_DIRECTX_ERROR_CHECK

#define DX_CHECK(call, result) 											\
	do																	\
	{																	\
		if (FAILED(result))												\
		{																\
			std::ostringstream oss;										\
			oss << call << " returned: " << result;						\
			std::string message = oss.str();							\
			throw new std::exception(message.c_str());					\
		}																\
	} while (0)

#else // !ENABLE_DIRECTX_ERROR_CHECK

#define DX_CHECK(call, result)

#endif // !ENABLE_DIRECTX_ERROR_CHECK



DirectXLayer::DirectXLayer(): m_d3d(NULL), m_device(NULL)
{
}

void DirectXLayer::CreateRenderingContext(const HWND& hWnd)
{
	assert(m_d3d == NULL);
	m_d3d = Direct3DCreate9( D3D_SDK_VERSION );

	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(params));
	params.AutoDepthStencilFormat = D3DFMT_D24X8;
	params.BackBufferCount = 1;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
	params.EnableAutoDepthStencil = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.Windowed = TRUE;
	params.hDeviceWindow = hWnd;

	assert(m_device == NULL);
	m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						hWnd,
						D3DCREATE_HARDWARE_VERTEXPROCESSING,
						&params,
						&m_device);
}

void DirectXLayer::DestroyRenderingContext()
{
	assert(m_device != NULL);
	m_device->Release();
	m_device = NULL;

	assert(m_d3d != NULL);
	m_d3d->Release();
	m_d3d = NULL;
}

VertexBufferID DirectXLayer::CreateVertexBuffer()
{
	VertexBufferInfo newVertexBuffer;
	newVertexBuffer.buffer = NULL;

	// Internal resource indexing
	m_vertexBuffers.add(newVertexBuffer);
	VertexBufferID id = { m_vertexBuffers.size - 1 };
	return id;
}

const VertexBufferID& DirectXLayer::LoadVertexBuffer(const VertexBufferID& id,
													 const VertexAttribute* vertexAttributes,
													 int numberOfAttributes, int stride,
													 int vertexDataSize, void* vertexData,
													 int indexDataSize, void* indexData,
													 VertexIndexType::Enum indexType)
{
	assert(m_vertexBuffers.size > id.index);
	VertexBufferInfo vertexBuffer = m_vertexBuffers[id.index];
	vertexBuffer.buffer = NULL;

	//
	// FIXME : have the layout depend on the vertexAttributes.
	// Have a custom init function to agree on the name of the attributes, then identify them here.
	//
	vertexBuffer.fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	vertexBuffer.stride = stride;
	vertexBuffer.primitiveCount = 12;

	// Creation of the buffer
	HRESULT createResult = m_device->CreateVertexBuffer(vertexDataSize, //4 * sizeof(CUSTOMVERTEX),
														0,
														vertexBuffer.fvf,
														D3DPOOL_MANAGED,
														&vertexBuffer.buffer,
														NULL);
	DX_CHECK("CreateVertexBuffer", createResult);

	// Copy of the vertex data
	void* p;
	HRESULT lockResult = vertexBuffer.buffer->Lock(0, 0, &p, 0);
	DX_CHECK("Lock", lockResult);

	memcpy(p, vertexData, vertexDataSize);
	vertexBuffer.buffer->Unlock();

	return id;
}

void DirectXLayer::DestroyVertexBuffer(const VertexBufferID& id)
{
	assert(m_vertexBuffers.size > id.index);
	IDirect3DVertexBuffer9* buffer = m_vertexBuffers[id.index].buffer = 0;
	m_vertexBuffers[id.index].buffer = NULL;
	m_vertexBuffers[id.index].primitiveCount = 0;

	if (buffer != NULL)
	{
		buffer->Release();
	}
}

TextureID DirectXLayer::CreateTexture()
{
	TextureInfo newTexture;
	newTexture.buffer = NULL;

	// Internal resource indexing
	m_textures.add(newTexture);
	TextureID id = { m_textures.size - 1 };
	return id;
}

const TextureID& DirectXLayer::LoadTexture(const TextureID& id,
										   int width, int height,
										   TextureType::Enum textureType,
										   TextureFormat::Enum textureFormat,
										   int side, int lodLevel,
										   void* data,
										   const TextureSampling& textureSampling)
{
	assert(m_textures.size > id.index);
	TextureInfo texture = m_textures[id.index];
	texture.buffer = NULL;

	// Creation of the texture
	HRESULT createResult = m_device->CreateTexture(width, height, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8,
												   D3DPOOL_MANAGED, &texture.buffer, NULL);
	DX_CHECK("CreateTexture", createResult);

	// Copy of the texture data
	D3DLOCKED_RECT d3drc;
	HRESULT lockResult = texture.buffer->LockRect(0, &d3drc, NULL, 0);
	DX_CHECK("LockRect", lockResult);

	memcpy(d3drc.pBits, data, width * height * sizeof(unsigned int));
	texture.buffer->UnlockRect(0);

	return id;
}

void DirectXLayer::DestroyTexture(const TextureID& id)
{
	IDirect3DTexture9* buffer = m_textures[id.index].buffer = 0;
	m_textures[id.index].buffer = NULL;

	if (buffer != NULL)
	{
		buffer->Release();
	}
}

ShaderID DirectXLayer::CreateShader()
{
	NOT_IMPLEMENTED;

	ShaderID id = { -1 };
	return id;
}

const ShaderID& DirectXLayer::LoadShader(const ShaderID& id,
										 const char* vertexShaderSource,
										 const char* vertexShaderSourceInfo,
										 const char* fragmentShaderSource,
										 const char* fragmentShaderSourceInfo)
{
	NOT_IMPLEMENTED;

	return id;
}

void DirectXLayer::DestroyShader(const ShaderID& id)
{
	NOT_IMPLEMENTED;
}

FrameBufferID DirectXLayer::CreateFrameBuffer(const TextureID* textures, int numberOfTextures)
{
	NOT_IMPLEMENTED;

	FrameBufferID id = { -1 };
	return id;
}

void DirectXLayer::DestroyFrameBuffer(const FrameBufferID& id)
{
	NOT_IMPLEMENTED;
}

void DirectXLayer::Clear(const FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth)
{
	const D3DCOLOR clearColor = D3DCOLOR_XRGB((int)(255 * r), (int)(255 * g), (int)(255 * b));
	m_device->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
	if (clearDepth)
	{
		m_device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	}

	HRESULT beginResult = m_device->BeginScene();
	DX_CHECK("BeginScene", beginResult);
}

/*
void DirectXLayer::SetCamera(int width, int height, float fov,
							 float yRotation, float distance)
{
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix,
							   D3DXToRadian(fov), (float)width / (float)height, 1.0f, 2.f * distance);
	m_device->SetTransform(D3DTS_PROJECTION, &projectionMatrix);

	D3DXMATRIX t;  D3DXMatrixTranslation(&t, 0.0f, 0.0f, distance);
	D3DXMATRIX rx; D3DXMatrixRotationX(&rx, D3DXToRadian(-20.0f));
	D3DXMATRIX ry; D3DXMatrixRotationY(&ry, D3DXToRadian(-yRotation));
	m_device->SetTransform(D3DTS_WORLD, &(ry * rx * t));

	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
}
*/

void DirectXLayer::Draw(const DrawArea& drawArea,
						const RasterTests& rasterTests,
						const Geometry& geometry,
						const ShadingParameters& shadingParameters)
{
	if (geometry.vertexBuffer.index >= 0)
	{
		const VertexBufferInfo& vertexBuffer = m_vertexBuffers[geometry.vertexBuffer.index];

		if (geometry.numberOfInstances <= 1)
		{
// 			if (vertexBuffer.indexBuffer != NULL)
			{
				m_device->SetFVF(vertexBuffer.fvf);
				m_device->SetStreamSource(0, vertexBuffer.buffer, 0, vertexBuffer.stride);
				m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vertexBuffer.primitiveCount);
			}
// 			else
// 			{
// 				m_device->SetFVF(vertexBuffer.fvf);
// 				m_device->SetStreamSource(0, vertexBuffer.buffer, 0, vertexBuffer.stride);
// 				m_device->SetIndices(vertexBuffer.indexBuffer);
// 				m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
// 											   vertexBuffer.vertexCount, 0, vertexBuffer.primitiveCount);
// 			}
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
}

void DirectXLayer::EndFrame()
{
	HRESULT endResult = m_device->EndScene();
	DX_CHECK("EndScene", endResult);

	HRESULT presentResult = m_device->Present(NULL, NULL, NULL, NULL);
	DX_CHECK("Present", presentResult);
}

#endif // defined(GFX_MULTI_API) || defined(GFX_DIRECTX_ONLY)

#endif // _WIN32
