#ifndef TEXTURE_HXX
#define TEXTURE_HXX

#include "Texture.hh"

namespace texture
{
	inline
	int Texture::Width() const
	{
		return m_width;
	}

	inline
	int Texture::Height() const
	{
		return m_height;
	}

	inline
	int Texture::FloatsPerPixel() const
	{
		return m_floatsPerPixel;
	}

	inline
	const float* Texture::Data() const
	{
		return m_data;
	}
}

#endif // TEXTURE_HXX
