#ifndef TEXTURE_UTILS_HH
#define TEXTURE_UTILS_HH

#include <cstddef>

namespace texture
{
	class Texture;

	/// <summary>
	/// Generates the normals from the heights. The pixel data
	/// format is unknown, but height is assumed to be a float and
	/// normal is assumed to be a Algebra::vector3f.
	/// heightOffset and normalOffset indicate their location in
	/// the structure.
	/// </summary>
	///
	/// <param name="parallaxScale">Ratio between a height of 1 and the width of the texture.</param>
	void GenerateNormalMap(texture::Texture& dest,
						   size_t heightOffset,
						   size_t normalOffset,
						   float parallaxScale);
	void GenerateFlowMap(texture::Texture& dest,
						 size_t heightOffset,
						 size_t flowOffset,
						 float scale);

#if DEBUG
	/// <summary>
	/// Exports a generated texture as a .tga (TARGA format).
	/// </summary>
	void ExportAsTARGA(const texture::Texture& texture,
					   const char* fileName,
					   int firstChannel,
					   int numberOfChannels);
#endif // DEBUG
}

#endif // TEXTURE_UTILS_HH
