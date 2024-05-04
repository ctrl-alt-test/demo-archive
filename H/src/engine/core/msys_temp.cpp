//
// Bits of code taken from IQ's 64k demo framework.
// http://www.iquilezles.org/code/framework64k/framework64k.htm
//

#include "msys_temp.hh"
#include <cstdlib>
#include <cstring>

void* msys_memdup(const void *src, size_t size)
{
	void* res = malloc(size);
	memcpy(res, src, size);
	return res;
}

int msys_ifloorf(float x)
{
	int res;

#ifdef _WIN32
static const short opc1 = 0x043f;     // floor
	short tmp;
	_asm fstcw   word  ptr [tmp]
	_asm fld     dword ptr [x]
	_asm fldcw   word  ptr [opc1]
	_asm fistp   dword ptr [res]
	_asm fldcw   word  ptr [tmp]
#else // !_WIN32
	res = (int)std::floor(x);
#endif // !_WIN32

	return res;
}
