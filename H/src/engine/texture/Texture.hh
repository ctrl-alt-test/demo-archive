#ifndef TEXTURE_HH
#define TEXTURE_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector4.hh"
#include <cassert>
#include <cstddef>

#define MAX_FLOATS_PER_PIXEL	32

namespace Noise
{
	class Rand;
}

namespace texture
{
	struct InputPixelInfo
	{
		const void*			inputPixelData;
		int					width;
		int					height;
		int					i;
		int					j;
		Algebra::vector2f	uv;
	};

	typedef void (*TextureFunction)(void* out_pixelData, const InputPixelInfo& input);

	typedef float (*DistanceFunction)(int x, int y);

	float ManhattanNorm(int x, int y);
	float EuclidianNorm(int x, int y);
	float ChebyshevNorm(int x, int y);

	class Texture
	{
	public:
		Texture();
		Texture(int with, int height, int floatsPerPixel = 4);
		~Texture();

		int				Width() const;
		int				Height() const;
		int				FloatsPerPixel() const;
		const float*	Data() const;

		int				GetIndex(int i, int j) const;
		int				GetClampedIndex(int i, int j) const;

		float*			Pixel(int i, int j);
		const float*	Pixel(int i, int j) const;
		void			Bilinear(float* out_fetch, float i, float j) const;

		void			Clear();

		void			PartialApplyFunction(TextureFunction function, int jmin, int jmax);

		/// <summary>
		/// Applies the function to each pixel of the texture.
		/// </summary>
		void			ApplyFunction(TextureFunction function);

		/// <summary>
		/// Applies the kernel to each pixel of the texture.
		/// </summary>
		void			ApplyConvolution(const float* kernel, int kernelWidth, int kernelHeight);

		void			FloodFill(unsigned x, unsigned y, const Algebra::vector4f& newColor);
		void			Line(float x0, float y0, float x1, float y1);
		void			Linei(int x0, int y0, int x1, int y1);

// 		void	AverageScale(unsigned int n, float coef);
// 		void	ZoomOut(float zx, float zy);
// 		void	subCopy(const Channel & a,
// 						unsigned int srcx, unsigned int srcy,
// 						unsigned int dstx, unsigned int dsty,
// 						unsigned int width, unsigned int height);

		void			Export(unsigned int* dest, int firstChannel, int numberOfChannels) const;
		void			ExportHDR(float* dest, int firstChannel, int numberOfChannels) const;

	private:
		int				m_width;
		int				m_height;
		int				m_floatsPerPixel;
		float*			m_data;
	};

	/// <summary>
	/// Meant to contain either a simple TextureFunction, 
	/// or either a complete function taking a Texture
	/// as a parameter for more complex generation.
	/// Yay duct tape polymorphism!
	/// </summary>
	struct TextureBuilder
	{
		texture::TextureFunction simpleFunction;
		void (*completeBuilder)(texture::Texture& dest);

		void Build(Texture& dest) const
		{
			if (simpleFunction != NULL)
			{
				assert(completeBuilder == NULL);
				dest.ApplyFunction(simpleFunction);
			}
			else
			{
				assert(simpleFunction == NULL);
				completeBuilder(dest);
			}
		}

		bool operator == (const TextureBuilder& rhs)
		{
			return simpleFunction == rhs.simpleFunction &&
				completeBuilder == rhs.completeBuilder;
		}
	};
}

#endif // TEXTURE_HH
