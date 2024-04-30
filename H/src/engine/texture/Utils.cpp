#include "Utils.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/msys_temp.hh"
#include "engine/texture/Texture.hxx"
#if DEBUG
#include <cstdio>
#endif // DEBUG

using namespace texture;

void texture::GenerateNormalMap(texture::Texture& dest,
								size_t heightOffset,
								size_t normalOffset,
								float parallaxScale)
{
	const int width = dest.Width();
	const int height = dest.Height();
	const float zScale = width * parallaxScale;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			const float* pixelBase = dest.Pixel(i, j);
			float height = *(pixelBase + heightOffset);

			// To estimate the normal at X, we take the average normal
			// of 1357 and the average normal of 0246. Using only 0246
			// tends to give more aliasing because we don't have enough
			// information to accurately estimate the slope of the
			// surface. Using 01234567 tends to give worse results than
			// just 0246.
			//
			// 3--2--1
			// | /|\ |
			// |/ | \|
			// 4--X--0
			// |\ | /|
			// | \|/ |
			// 5--6--7

			// SIZE-CODING:
			// Writing in this obfuscated way saves ~800 bytes on the
			// non compressed binary, and ~100 bytes on the compressed
			// binary, compared to the naive version after.
#if 1
			Algebra::vector3f p[8];
			const char offsets[16] = {
				+1, 0,
				+1, +1,
				0, +1,
				-1, +1,
				-1, 0,
				-1, -1,
				0, -1,
				-1, -1
			};
			for (int k = 0; k < 8; ++k)
			{
				p[k].x = float(offsets[2 * k]);
				p[k].y = float(offsets[2 * k + 1]);
				p[k].z = zScale * (*(dest.Pixel(i + offsets[2 * k], j + offsets[2 * k + 1]) + heightOffset) - height);
			}

			Algebra::vector3f normal = Algebra::vector3f::zero;
			for (int k = 0; k < 8; ++k)
			{
				int index = 2 * k + (k >= 4);
				normal += cross(p[index % 8], p[(index + 2) % 8]);
			}
			*(Algebra::vector3f*)(pixelBase + normalOffset) = normalized(normal) * 0.5f + 0.5f;
#else
			const Algebra::vector3f p0 = { +1.f,  0.f, zScale * (*(dest.Pixel(i + 1, j    ) + heightOffset) - height) };
			const Algebra::vector3f p1 = { +1.f, +1.f, zScale * (*(dest.Pixel(i + 1, j + 1) + heightOffset) - height) };
			const Algebra::vector3f p2 = {  0.f, +1.f, zScale * (*(dest.Pixel(i    , j + 1) + heightOffset) - height) };
			const Algebra::vector3f p3 = { -1.f, +1.f, zScale * (*(dest.Pixel(i - 1, j + 1) + heightOffset) - height) };
			const Algebra::vector3f p4 = { -1.f,  0.f, zScale * (*(dest.Pixel(i - 1, j    ) + heightOffset) - height) };
			const Algebra::vector3f p5 = { -1.f, -1.f, zScale * (*(dest.Pixel(i - 1, j - 1) + heightOffset) - height) };
			const Algebra::vector3f p6 = {  0.f, -1.f, zScale * (*(dest.Pixel(i    , j - 1) + heightOffset) - height) };
			const Algebra::vector3f p7 = { +1.f, -1.f, zScale * (*(dest.Pixel(i + 1, j - 1) + heightOffset) - height) };

			Algebra::vector3f& normal = *(Algebra::vector3f*)(pixelBase + normalOffset);
			normal = normalized(
				// The diamond
				cross(p0, p2) + cross(p2, p4) + cross(p4, p6) + cross(p6, p0) +
				// The square
				cross(p1, p3) + cross(p3, p5) + cross(p5, p7) + cross(p7, p1)) * 0.5f + 0.5f;
#endif
		}
	}
}

void texture::GenerateFlowMap(texture::Texture& dest,
							  size_t heightOffset,
							  size_t flowOffset,
							  float scale)
{
	const int width = dest.Width();
	const int height = dest.Height();
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			const char SobelWeightsAndOffsets[] = {
				+1,		-1, -1,
				+2,		-1,  0,
				+1,		-1, +1,

				-1,		+1, -1,
				-2,		+1,  0,
				-1,		+1, +1,
			};
			float gx = 0.f;
			float gy = 0.f;
			for (int k = 0; k < ARRAY_LEN(SobelWeightsAndOffsets); k += 3)
			{
				const float weight = SobelWeightsAndOffsets[k] / 8.f;
				const char a = SobelWeightsAndOffsets[k + 1];
				const char b = SobelWeightsAndOffsets[k + 2];

				gx += *(dest.Pixel(i + a, j + b) + heightOffset) * weight;
				gy += *(dest.Pixel(i + b, j + a) + heightOffset) * weight;
			}

			Algebra::vector2f flowDirection = { gy, -gx };
			*(Algebra::vector2f*)(dest.Pixel(i, j) + flowOffset) = flowDirection * scale * 0.5f + 0.5f;
		}
	}
}

#if DEBUG
void texture::ExportAsTARGA(const texture::Texture& texture,
							const char* fileName,
							int firstChannel,
							int numberOfChannels)
{
	assert(numberOfChannels > 0 && numberOfChannels <= 4);
	assert(firstChannel + numberOfChannels <= texture.FloatsPerPixel());

	FILE* fp = fopen(fileName, "wb");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', texture won't be exported.", fileName);
	}
	else
	{
		// TARGA format description:
		// http://www.paulbourke.net/dataformats/tga/

		const int width = texture.Width();
		const int height = texture.Height();
		const int floatPerPixel = texture.FloatsPerPixel();
		const float* data = texture.Data();

		const unsigned char header[] = {
			0,
			0, // Color map type (none).
			2, // Image type (unmapped RGB).
			0, 0, 0, 0, 0, // Color map specification (ignored).
			0, 0, 0, 0, // Origin of the image (0, 0).
			(unsigned char)(width & 0x00FF), (unsigned char)((width & 0xFF00) / 256),
			(unsigned char)(height & 0x00FF), (unsigned char)((height & 0xFF00) / 256),
			32, // Image pixel size (32 bpp).
			8, // Image descriptor type.
		};
		assert(sizeof(header) == 18);
		fwrite(header, 1, sizeof(header), fp);

		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				const float* color = &data[firstChannel + (i + width * j) * floatPerPixel];
				unsigned char r =                             (unsigned char)msys_ifloorf(Algebra::clamp(color[0]) * 255.0f);
				unsigned char g = (numberOfChannels < 2 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[1]) * 255.0f));
				unsigned char b = (numberOfChannels < 3 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[2]) * 255.0f));
				unsigned char a = (numberOfChannels < 4 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[3]) * 255.0f));
				fputc(b, fp);
				fputc(g, fp);
				fputc(r, fp);
				fputc(a, fp);
			}
		}

		//const char footer[] =
		//	"\0\0\0\0"
		//	"\0\0\0\0"
		//	"TRUEVISION-XFILE.";
		//fwrite(footer, sizeof(footer), 1, fp);
		fclose(fp);
	}
}
#endif // DEBUG
