#include "Hash.hh"

#include "engine/core/msys_temp.hh"

#define HARD_CODED_HASH 0 // 0 to generate the hash with Rand,
                          // 1 to use the hard coded hash.

#if (!HARD_CODED_HASH)
#include "Rand.hh"
#include "engine/container/Algorithm.hxx"
#include "engine/container/Array.hh"
#endif

#include "engine/container/Algorithm.hh"

using namespace Noise;

//
// Notes:
//
// In the original noise implementation, K. Perlin uses a 8 bits hash,
// and uses a bitmask rather than a modulo. If we constrain the array
// size, we can use the same micro-optimization.
// (in get(), "i % count" becomes "i & mask")
//
//
// Interesting read on the different hash functions:
// http://programmers.stackexchange.com/a/145633/19537
//

#if HARD_CODED_HASH

const int hash[] = {
	151, 160, 137,  91, 90, 15, 131,  13, 201, 95, 96, 53,  194, 233, 7,
	225, 140, 36, 103, 30, 69, 142,  8, 99, 37, 240, 21, 10, 23, 190, 6,
	148, 247, 120, 234, 75, 0, 26,  197, 62, 94, 252, 219, 203, 117, 35,
	11, 32, 57, 177,  33, 88, 237, 149, 56, 87, 174,  20, 125, 136, 171,
	168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231,
	83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245,
	40, 244,  102, 143, 54, 65,  25, 63, 161,  1, 216, 80, 73,  209, 76,
	132, 187, 208,  89, 18, 169, 200, 196, 135, 130,  116, 188, 159, 86,
	164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5,
	202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16,
	58, 17, 182, 189, 28, 42, 223,  183, 170, 213, 119, 248, 152, 2, 44,
	154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
	19, 98, 108,  110, 79, 113, 224, 232, 178, 185,  112, 104, 218, 246,
	97, 228, 251,  34, 242, 193, 238, 210, 144, 12,  191, 179, 162, 241,
	81, 51,  145, 235, 249,  14, 239, 107,  49, 192, 214, 31,  181, 199,
	106, 157,  184, 84, 204,  176, 115, 121,  50, 45, 127, 4,  150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78,
	66, 215, 61, 156, 180
};
const int Hash::mask = ARRAY_LEN(hash);

void Hash::init() {}

#else // !HARD_CODED_HASH

int* hash = NULL;
const int Hash::mask = 0xff;

void Hash::init()
{
	assert(hash == NULL);
	hash = (int*)malloc((0xff + 1) * sizeof(int));

	// All values from 0 to mask, in random order.
	for (int i = 0; i <= mask; ++i)
	{
		hash[i] = i;
	}
	Rand rnd;
	Container::shuffle(hash, rnd, 0, mask);
}

#endif // !HARD_CODED_HASH

int Hash::get8(int i)
{
	if (hash == NULL) // FIXME: HACK for RCC++, the pointer is lost when used from a DLL.
	{
		init();
	}

	return hash[i & mask];
}

int Hash::get8(int i, int j)
{
	return get8(get8(i) + j);
}

int Hash::get8(int i, int j, int k)
{
	return get8(get8(get8(i) + j) + k);
}

int Hash::get8(int i, int j, int k, int l)
{
	return get8(get8(get8(get8(i) + j) + k) + l);
}

unsigned int Hash::get32(int i)
{
	// In theory, C++ says this is a typical reinterpret_cast use case.
	// In practice though, its implementation in unspecified, and the
	// static code analysis tool complains. So we use static_cast
	// instead. See:
	// http://stackoverflow.com/questions/1863069/casting-via-void-instead-of-using-reinterpret-cast
	return get32(static_cast<const void*>(&i), sizeof(int));
}

unsigned int Hash::get32(int i, int j)
{
	int data[2] = { i, j };
	return get32(data, sizeof(data));
}

unsigned int Hash::get32(int i, int j, int k)
{
	int data[3] = { i, j, k };
	return get32(data, sizeof(data));
}

unsigned int Hash::get32(int i, int j, int k, int l)
{
	int data[4] = { i, j, k, l };
	return get32(data, sizeof(data));
}

unsigned int Hash::get32(float x)
{
	return get32(static_cast<const void*>(&x), sizeof(x));
}

unsigned int Hash::get32(float x, float y)
{
	float data[2] = { x, y };
	return get32(data, sizeof(data));
}

unsigned int Hash::get32(float x, float y, float z)
{
	float data[3] = { x, y, z };
	return get32(data, sizeof(data));
}

unsigned int Hash::get32(float x, float y, float z, float w)
{
	float data[4] = { x, y, z, w };
	return get32(data, sizeof(data));
}

#if ENABLE_SBDM // Disabled because it results in noticeable patterns.
// This algorithm was created for sdbm (a public-domain
// reimplementation of ndbm) database library.
//
// Source:
// http://www.cse.yorku.ca/~oz/hash.html
//
static unsigned int sdbm(const unsigned char *str)
{
	unsigned int h = 0;
	while (const int c = *str++)
	{
		h = c + (h << 6) + (h << 16) - h;
	}

	return h;
}

static unsigned int sdbm(const void* ptr, int size)
{
	const unsigned char* str = (const unsigned char*)ptr;

	unsigned int h = 0;
	while (size-- > 0)
	{
		const int c = *str++;
		h = c + (h << 6) + (h << 16) - h;
	}

	return h;
}
#endif // ENABLE_SBDM

// Fowler–Noll–Vo hash, 1a variant.
//
// References:
// http://isthe.com/chongo/tech/comp/fnv/
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
// http://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
//
#define FNV_32B_PRIME			16777619
#define FNV_32B_OFFSET_BASIS	2166136261

static unsigned int fnv1a(const unsigned char* str)
{
	unsigned int hash = FNV_32B_OFFSET_BASIS;

	while (int c = *str++)
	{
		c *= 0xff; // This is not part of the hash, but it gives better results in my tests.
		hash ^= c;
		hash *= FNV_32B_PRIME;
	}
	return hash;
}

static unsigned int fnv1a(const void* ptr, int size)
{
	unsigned int hash = FNV_32B_OFFSET_BASIS;

	const unsigned char* str = (const unsigned char*)ptr;
	while (size-- > 0)
	{
		int c = *str++;
		hash ^= c;
		hash *= FNV_32B_PRIME;
	}
	return hash;
}

unsigned int Hash::get32(const unsigned char* str)
{
	// I might be doing something wrong, but in our typical use case,
	// hash of texture coordinates, some patterns are visible. Hashing
	// a second time fixes this though.
	unsigned int hash1 = fnv1a(str);
	return fnv1a(&hash1, sizeof(unsigned int));
}

unsigned int Hash::get32(const char* str)
{
	return get32((const unsigned char*)str);
}

unsigned int Hash::get32(const void* ptr, int size)
{
	// I might be doing something wrong, but in our typical use case,
	// hash of texture coordinates, some patterns are visible. Hashing
	// a second time fixes this though.
	unsigned int hash1 = fnv1a(ptr, size);
	return fnv1a(&hash1, sizeof(unsigned int));
}
