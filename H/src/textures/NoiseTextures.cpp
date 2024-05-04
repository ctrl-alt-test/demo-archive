#include "NoiseTextures.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/core/msys_temp.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/LatticeNoise.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/Rand.hh"
#include "engine/noise/SimplexNoise.hh"
#include "engine/noise/SimplexValueNoise.hh"
#include "engine/noise/ValueNoise.hh"
#include "engine/noise/Utils.hh"
#include "engine/texture/Texture.hxx"

using namespace Algebra;

void LatticeNoise(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const float x = Noise::LatticeNoise::value(in.uv * 10.f, 10.0f);

	const vector4f sValue = { x, x, x, 1 };
	const vector4f usValue = { msys_max(x, 0.0f), msys_max(-x, 0.0f), 0, 1 };
	*(vector4f*)out_pixelData = (in.i > in.j ? sValue : usValue);
}

void ValueNoise(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const float x = Noise::ValueNoise::value(in.uv * 10.f, 10.0f);

	const vector4f sValue = { x, x, x, 1 };
	const vector4f usValue = { msys_max(x, 0.0f), msys_max(-x, 0.0f), 0, 1 };
	*(vector4f*)out_pixelData = (in.i > in.j ? sValue : usValue);
}

void FbmPerlinNoise(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const float x = Noise::fBmPerlin(in.uv * 4.f, 4.f, 5, 2.f, 2.f);
	const vector4f sValue = { x, x, x, 1 };
	const vector4f usValue = { msys_max(x, 0.0f), msys_max(-x, 0.0f), 0, 1 };
	*(vector4f*)out_pixelData = (in.i > in.j ? sValue : usValue);
}

void SimplexValueNoise(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const float x = Noise::SimplexValueNoise::value(in.uv * 10.f);

	const vector4f sValue = { x, x, x, 1 };
	const vector4f usValue = { msys_max(x, 0.0f), msys_max(-x, 0.0f), 0, 1 };
	*(vector4f*)out_pixelData = (in.i > in.j ? sValue : usValue);
}

void SimplexNoise(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const float x = Noise::SimplexNoise::value(in.uv * 10.0f);

	const vector4f sValue = { x, x, x, 1 };
	const vector4f usValue = { msys_max(x, 0.0f), msys_max(-x, 0.0f), 0, 1 };
	*(vector4f*)out_pixelData = (in.i > in.j ? sValue : usValue);
}

void SphereNormalMap(void* out_pixelData, const texture::InputPixelInfo& in)
{
	const vector2f uv = 2.f * uv - 1.f;
	float d = norm(uv);

	vector2f xy = uv * msys_sinf(d);
	vector3f n = { xy.x, xy.y, msys_cosf(d) };
	n *= smoothStep(1.01f, 1.0f, d);
	vector3f rgb = 0.5f * n + 0.5f;

	vector4f result = { rgb.x, rgb.y, rgb.z, smoothStep(1.01f, 1.0f, d) };
	*(vector4f*)out_pixelData = result;
}
