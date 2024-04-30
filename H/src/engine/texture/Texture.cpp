#include "Texture.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/container/Array.hxx"
#include "engine/core/Debug.hh"
#include "engine/noise/Rand.hh"

#include "engine/core/msys_temp.hh"
#include <cassert>
#include <cstring>

// FIXME: this code is written here as a quick and dirty hack, but we
// don't want low level implementation in this file, so this entire
// block should be removed when possible.
// ---8<---------------------------------------------------------------
#if _WIN32
#include <Windows.h>

static int getNumberOfCores()
{
	static int numberOfCores = -1;
	if (numberOfCores > 0)
	{
		return numberOfCores;
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	numberOfCores = sysinfo.dwNumberOfProcessors;
	return numberOfCores;
}

typedef HANDLE ThreadData;
typedef void (__cdecl *ThreadFunc)(void*);
typedef void* ThreadArg;

struct Thread
{
	static void StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg)
	{
		*threadData = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
	}

	static void WaitAllThreads(const ThreadData* threads, int numberOfThreads)
	{
		WaitForMultipleObjects(numberOfThreads, threads, TRUE, INFINITE);
	}
};

#else // !_WIN32
#include <pthread.h>
#include <unistd.h>

static int getNumberOfCores()
{
	static int numberOfCores = -1;
	if (numberOfCores > 0)
	{
		return numberOfCores;
	}

	numberOfCores = sysconf(_SC_NPROCESSORS_ONLN);
	return numberOfCores;
}

typedef pthread_t ThreadData;
typedef void (*ThreadFunc)(void*);
typedef void* ThreadArg;

//
// FIXME: this probably doesn't work because I have no idea what I'm
// doing. At all.
//
struct Thread
{
	static void StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg)
	{
		pthread_create(threadData, NULL, (void*(*)(void*))func, arg);
	}

	static void WaitAllThreads(const ThreadData* threads, int numberOfThreads)
	{
		for (int i = 0; i < numberOfThreads; ++i)
		{
			pthread_join(threads[i], NULL);
		}
	}
};

#endif // !_WIN32
// --------------------------------------------------------------->8---

using namespace texture;

Texture::Texture():
	m_width(0),
	m_height(0),
	m_floatsPerPixel(0),
	m_data(NULL)
{
}

Texture::Texture(int width, int height, int floatsPerPixel):
	m_width(width),
	m_height(height),
	m_floatsPerPixel(floatsPerPixel)
{
	assert(floatsPerPixel > 0 && floatsPerPixel <= MAX_FLOATS_PER_PIXEL);
	m_data = new float[width * height * floatsPerPixel];
}

Texture::~Texture()
{
	delete[] m_data;
}

int Texture::GetIndex(int i, int j) const
{
	return imod(i, m_width) + imod(j, m_height) * m_width;
}

int Texture::GetClampedIndex(int i, int j) const
{
	return Algebra::clamp(i, 0, m_width - 1) + Algebra::clamp(j, 0, m_height - 1) * m_width;
}

float* Texture::Pixel(int i, int j)
{
	return &m_data[GetIndex(i, j) * m_floatsPerPixel];
}

const float* Texture::Pixel(int i, int j) const
{
	return &m_data[GetIndex(i, j) * m_floatsPerPixel];
}

void Texture::Bilinear(float* out_sample, float x, float y) const
{
	int i = msys_ifloorf(x);
	int j = msys_ifloorf(y);

	float i_ratio = i - (float)(x);
	float j_ratio = j - (float)(y);

	const float* p00 = Pixel(i    , j    );
	const float* p10 = Pixel(i + 1, j    );
	const float* p01 = Pixel(i    , j + 1);
	const float* p11 = Pixel(i + 1, j + 1);

	for (int f = 0; f < m_floatsPerPixel; ++f)
	{
		out_sample[f] =
			Algebra::mix(Algebra::mix(p00[f], p10[f], i_ratio),
						 Algebra::mix(p01[f], p11[f], i_ratio), j_ratio);
	}
}

void Texture::Clear()
{
	memset(m_data, 0, m_width * m_height * m_floatsPerPixel * sizeof(float));
}

float texture::ManhattanNorm(int x, int y)
{
	return (float)(x + y);
}

float texture::EuclidianNorm(int x, int y)
{
	Algebra::vector2f v = { (float)x, (float)y };
	return Algebra::norm(v);
}

float texture::ChebyshevNorm(int x, int y)
{
	return (float)msys_max(x, y);
}

// ================================================================== Filters

// ------------------------------------------------------ Histogram functions

/*
// fait une coupure, juste avant b
static float oneCut(float x, float a, float b)
{
	if (x < a || x > b) return x;

	const float cut = b - 0.01f;
	if (x < cut)
		return Algebra::interpolate(x, a, cut);
	else
		return Algebra::interpolate(x, b, cut);
}

static float cut(float x, int n)
{
	const int div = n + 1; // nombre de sections
	for (int i = 0; i < n; i++)
		if (x < (float)(i+1) / div)
			return oneCut(x, (float)i / div, (float)(i+1) / div);
	return Algebra::interpolate(x, (float)n / div, 1.f);
}

// Effectue un modulo sur un flottant pour le remettre entre 0 et 1.
// C'est une fonction continue : mod(1.1) == mod(0.9)
static float mod(float x)
{
	x = Algebra::abs(x);
	const int ipart = msys_ifloorf(x);
	const float fpart = x - (float)ipart;
	if (ipart % 2 == 1)
		return 1 - fpart;
	else
		return fpart;
}
*/

// ---------------------------------------------------------- Pixel functions

#if 0 // FIXME: to do
void Texture::FloodFill(unsigned x, unsigned y, float newColor)
{
	Container::Array<int> stack(10000);

	float oldColor = (*this)[x + y * m_width];
	stack.add(x + y * m_width);

	while (stack.size > 0)
	{
		bool spanLeft = false;
		bool spanRight = false;
		int pos = stack.last();
		stack.pop();
		x = imod(pos % m_width, m_width);
		y = imod(pos / m_width, m_width);
		int y1 = y;

		while ((y != m_height + y1 - 1) && (*this)[GetIndex(x, y1)] == oldColor)
		{
			y1--;
		}
		y1 = imod(y1 + 1, m_width);

		while ((*this)[GetIndex(x, y1)] == oldColor)
		{
			(*this)[GetIndex(x, y1)] = newColor;
			if (spanLeft == false && ((*this)[GetIndex(x - 1, y1)] == oldColor))
			{
				stack.add(imod(x - 1, m_width) + y1 * m_width);
				spanLeft = true;
			}
			else if(spanLeft == true && ((*this)[GetIndex(x - 1, y1)] != oldColor))
			{
				spanLeft = false;
			}
			if (spanRight == false && ((*this)[GetIndex(x + 1, y1)] == oldColor))
			{
				stack.add(imod(x + 1, m_width) + y1 * m_width);
				spanRight = true;
			}
			else if (spanRight == true && ((*this)[GetIndex(x + 1, y1)] != oldColor))
			{
				spanRight = false;
			}
			y1 = (y1 + 1) % m_width;
		}
	}
}

// Bresenham algorithm
void Texture::Line(float x0, float y0, float x1, float y1)
{
	int xi0 = msys_ifloorf(x0 * m_width);
	int yi0 = msys_ifloorf(y0 * m_height);
	int xi1 = msys_ifloorf(x1 * m_width);
	int yi1 = msys_ifloorf(y1 * m_height);
	Linei(xi0, yi0, xi1, yi1);
}

// Bresenham algorithm
void Texture::Linei(int x0, int y0, int x1, int y1)
{
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;
	int ystep = (y0 < y1) ? 1 : -1;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		(*this)[steep ? GetIndex(y, x) : GetIndex(x, y)] = 1.f;
		error = error - deltay;
		if (error < 0)
		{
			y += ystep;
			error += deltax;
		}
	}
}
#endif

// --------------------------------------------------------------------------

void Texture::PartialApplyFunction(TextureFunction function, int jmin, int jmax)
{
	float outputPixelData[MAX_FLOATS_PER_PIXEL];
	for (int j = jmin; j < jmax; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			float* pixelData = &m_data[(i + j * m_width) * m_floatsPerPixel];

			InputPixelInfo inputInfo = {
				pixelData,
				m_width,
				m_height,
				i,
				j,
				{ (i + 0.5f) / (float)m_width, (j + 0.5f) / (float)m_height }
			};

			function((void*)&outputPixelData, inputInfo);
			memcpy(pixelData, outputPixelData, m_floatsPerPixel * sizeof(float));
		}
	}
}

struct PartialFunctionData
{
	Texture* t;
	TextureFunction function;
	int jmin;
	int jmax;
};

// Wrapper, because CreateThread can pass only a void*
static void __cdecl PartialApplyFunctionWrapper(void* args)
{
	PartialFunctionData* data = (PartialFunctionData*)args;
	data->t->PartialApplyFunction(data->function, data->jmin, data->jmax);
}

void Texture::ApplyFunction(TextureFunction function)
{
	assert(m_floatsPerPixel <= MAX_FLOATS_PER_PIXEL);

	const int MAX_THREADS = 128; // At 256 we hit the __chkstk link error.
	const int threadsNumber = msys_max(1, msys_min(MAX_THREADS, getNumberOfCores()));
	LOG_INFO("texture: using %d threads", threadsNumber);
	ThreadData threads[MAX_THREADS];
	PartialFunctionData args[MAX_THREADS];

	int start = 0;
	int size = m_height / threadsNumber;
	for (int i = 0; i < threadsNumber; i++)
	{
		int end = i + 1 == threadsNumber ? m_height : start + size;
		PartialFunctionData arg = { this, function, start, end };
		args[i] = arg;
		Thread::StartThread(&threads[i], PartialApplyFunctionWrapper, &args[i]);
		start = end;
	}

	Thread::WaitAllThreads(threads, threadsNumber);
}

void Texture::ApplyConvolution(const float* kernel, int kernelWidth, int kernelHeight)
{
	assert(m_floatsPerPixel <= MAX_FLOATS_PER_PIXEL);
	const Texture save(*this);

	float factor = 0;
	for (int i = 0; i < kernelWidth * kernelHeight; ++i)
	{
		factor += kernel[i];
	}
	factor = (factor == 0 ? 1.f : 1.f / factor);

	for (int j = 0; j < m_height; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			float sum[MAX_FLOATS_PER_PIXEL] = { 0.f };
			for (int dj = 0; dj < kernelHeight; ++dj)
			{
				for (int di = 0; di < kernelWidth; ++di)
				{
					float weight = kernel[di + kernelHeight * dj];
					int index = GetIndex(i + di, j + dj);
					for (int f = 0; f < m_floatsPerPixel; ++f)
					{
						sum[f] += weight * m_data[index * m_floatsPerPixel + f];
					}
				}
			}

			int writeIndex = GetIndex(i + kernelWidth / 2, j + kernelHeight / 2);
			for (int f = 0; f < m_floatsPerPixel; ++f)
			{
				m_data[writeIndex * m_floatsPerPixel + f] = factor * sum[f];
			}
		}
	}
}

/*
static const float bumpOperator[9] = { 0, -1, 0,
									   -1,  2, 0,
									   0,  0, 0 };

static const float embossOperator[9] = { -2, -1, 0,
										 -1,  1, 1,
										 0,  1, 2 };

static const float SobelFeldmanSharrVerticalOperator[9] = { +3,  +10, +3,
															0, 0, 0,
															-3,  -10, -3 };

static const float SobelFeldmanSharrHorizontalOperator[9] = { +3,  0, -3,
															  +10, 0, -10,
															  +3,  0, -3 };
static const float sharpenOperator[9] = { 0, -1, 0,
										  -1, 5, -1,
										  0, -1, 0 };

static const float blurOperator[9] = { 0, 1, 0,
									   1, 4, 1,
									   0, 1, 0 };

static const float gaussianOperator[25] = { 1,  4,  7,  4, 1 };
*/
// --------------------------------------------------------------------------

#if 0 // FIXME: to do
void Texture::Dilate(float radius)
{
	assert(radius > 0 && radius <= 0.5f);
	const int iradius = 1 + (unsigned int)(m_width * radius);
	const float invMaxDistance = 1.f / msys_sqrtf(float(iradius * iradius));

	const Texture save(*this);

	for (unsigned int j = 0; j < m_height; ++j)
	{
		for (unsigned int i = 0; i < m_width; ++i)
		{
			float maxValue = 0;

			for (int kj = 0; kj < iradius; ++kj)
			{
				for (int ki = 0; ki < iradius; ++ki)
				{
					const float factor = 1.f - msys_min(1.f, msys_sqrtf(float(ki * ki + kj * kj)) * invMaxDistance);
					maxValue = msys_max(maxValue, factor * save[GetIndex(i + ki, j + kj)]);
					maxValue = msys_max(maxValue, factor * save[GetIndex(i - ki, j + kj)]);
					maxValue = msys_max(maxValue, factor * save[GetIndex(i + ki, j - kj)]);
					maxValue = msys_max(maxValue, factor * save[GetIndex(i - ki, j - kj)]);
				}
			}

			(*this)[i + j * m_width] = maxValue;
		}
	}
}

void Texture::Transpose()
{
	// FIXME : refaire sans allouer de nouvelle texture

	const Texture save(*this);

	for (unsigned int j = 0; j < m_height; ++j)
	{
		const unsigned int shift = j * m_width;
		for (unsigned int i = 0; i < m_width; ++i)
		{
			(*this)[i + shift] = save[j + m_height * i];
		}
	}
}

void Texture::Rotate90()
{
	// FIXME : refaire sans allouer de nouvelle texture

	const Texture save(*this);

	for (unsigned int j = 0; j < m_height; ++j)
	{
		const unsigned int shift = j * m_width;
		for (unsigned int i = 0; i < m_width; ++i)
		{
			(*this)[i + shift] = save[j + m_width * (m_height - i - 1)];
		}
	}
}

void Texture::AverageScale(unsigned int n, float coef)
{
	assert(coef != 0);
	const Texture save(*this);
	int mult = 1;
	float c = 1.f;
	for (unsigned int k = 0; k < n; k++)
	{
		mult *= 2;
		c *= coef;
		const float invC = 1.f / c;
		for (unsigned int j = 0; j < m_height; ++j)
		{
			const unsigned int shift = j * m_width;
			for (unsigned int i = 0; i < m_width; ++i)
			{
				(*this)[i + shift] += save[GetIndex(i*mult, j*mult)] * invC;
			}
		}
	}
}

void Texture::ZoomOut(float zx, float zy)
{
	const Texture save(*this);
	for (unsigned int j = 0; j < m_height; ++j)
	{
		const unsigned int shift = j * m_width;
		for (unsigned int i = 0; i < m_width; ++i)
		{
			(*this)[i + shift] = save.Bilinear(i * zx, j * zy);
		}
	}
}

void Texture::subCopy(const Texture& a,
					  unsigned int srcx, unsigned int srcy,
					  unsigned int dstx, unsigned int dsty,
					  unsigned int width, unsigned int height)
{
	assert(srcx + width <= a.Width());
	assert(srcy + height <= a.Height());
	assert(dstx + width <= m_width);
	assert(dsty + height <= m_height);

	for (unsigned int j = 0; j < height; ++j)
	{
		const unsigned int srcShift = (srcy + j) * a.Width();
		const unsigned int dstShift = (dsty + j) * m_width;

		for (unsigned int i = 0; i < width; ++i)
		{
			(*this)[dstx + i + dstShift] = a[srcx + i + srcShift];
		}
	}
}
#endif

// ---8<----------------------------------------------------------------------
// Export, for standalone texgen

void Texture::Export(unsigned int* dest, int firstChannel, int numberOfChannels) const
{
	assert(numberOfChannels > 0 && numberOfChannels <= 4);
	assert(firstChannel + numberOfChannels <= m_floatsPerPixel);

	for (int j = 0; j < m_height; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			const float* color = &m_data[firstChannel + (i + m_width * j) * m_floatsPerPixel];
			unsigned char r =                             (unsigned char)msys_ifloorf(Algebra::clamp(color[0]) * 255.0f);
			unsigned char g = (numberOfChannels < 2 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[1]) * 255.0f));
			unsigned char b = (numberOfChannels < 3 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[2]) * 255.0f));
			unsigned char a = (numberOfChannels < 4 ? 0 : (unsigned char)msys_ifloorf(Algebra::clamp(color[3]) * 255.0f));

			// FIXME: assumes RGBA8 texture format for the moment.
			dest[m_width * j + i] = a << 24 | b << 16 | g << 8 | r;
		}
	}
}

void Texture::ExportHDR(float* dest, int firstChannel, int numberOfChannels) const
{
	assert(numberOfChannels > 0 && numberOfChannels <= 4);
	assert(firstChannel + numberOfChannels <= m_floatsPerPixel);

	for (int j = 0; j < m_height; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			const float* color = &m_data[firstChannel + (i + m_width * j) * m_floatsPerPixel];
			dest[4 * (m_width * j + i) + 0] =                               color[0];
			dest[4 * (m_width * j + i) + 1] = (numberOfChannels < 2 ? 0.f : color[1]);
			dest[4 * (m_width * j + i) + 2] = (numberOfChannels < 3 ? 0.f : color[2]);
			dest[4 * (m_width * j + i) + 3] = (numberOfChannels < 4 ? 0.f : color[3]);
		}
	}
}
