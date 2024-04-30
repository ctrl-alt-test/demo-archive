#include "RCCTest.hh"
#include "engine/algebra/Interpolation.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hh"
#include "engine/container/Utils.hh"
#include "engine/texture/Texture.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(SomeTexture);
SomeTexture* SomeTexture::instance = NULL;
void SomeTexture::BuildCaller(void* out_pixelData, const texture::InputPixelInfo& in)
{
	return instance->Build(out_pixelData, in);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int SomeTexture::s_layerSizes[] = { 4, 4, };
const int SomeTexture::s_numberOfLayers = ARRAY_LEN(SomeTexture::s_layerSizes);
const float SomeTexture::s_parallaxScale = 0.f;

void SomeTexture::Build(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const Algebra::vector3f teal =   { 0.05f, 0.21f, 0.46f };
	const Algebra::vector3f orange = { 1.00f, 0.49f, 0.07f };
	const Algebra::vector3f color = mix(teal, orange, in.uv.x);

	struct pixelFormat
	{
		Algebra::vector4f c1;
		Algebra::vector4f c2;
	};
	pixelFormat result = { { color.x, color.y, color.z, 1, }, { 1.f, 0.f, .4f, 0.5f, } };
	*(static_cast<pixelFormat*>(out_pixelData)) = result;
}
