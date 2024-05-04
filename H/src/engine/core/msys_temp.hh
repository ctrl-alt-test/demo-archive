//
// Bits of code taken from IQ's 64k demo framework.
// http://www.iquilezles.org/code/framework64k/framework64k.htm
//

#ifndef MSYS_TEMP_HH
#define MSYS_TEMP_HH

#include <cmath>
#include <cstddef>

int		msys_ifloorf(float x);

#define msys_acosf(x)	std::acos(x)
#define msys_atan2f(y, x) atan2f(y, x)
#define msys_cosf(x)	std::cos(x)
#define msys_sinf(x)	std::sin(x)
#define msys_tanf(x)	std::tan(x)
#define msys_sqrtf(x)	std::sqrt(x)

void*	msys_memdup(const void *src, size_t size);

// Modulo toujours positif.
inline
int imod(int i, int n)
{
	return i >= 0 ? (i % n) : ((i + 1) % n + n - 1);
}

template<typename T>
const T& msys_max(const T& a, const T& b)
{
	return a >= b ? a : b;
}

template<typename T>
const T& msys_min(const T& a, const T& b)
{
	return a <= b ? a : b;
}

#endif // MSYS_TEMP_HH
