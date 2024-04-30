#ifndef HASH_HH
#define HASH_HH

namespace Noise
{
	// FIXME: est-ce qu'on veut un truc global comme ça, ou plutôt un
	//        objet comme pour rand() ?
	class Hash
	{
	public:
		static const int mask;

		// Initializes the hash.
		static void init();

		// Gets a hash value in [0..255] from 1 to 4 integers.
		static int get8(int i);
		static int get8(int i, int j);
		static int get8(int i, int j, int k);
		static int get8(int i, int j, int k, int l);

		// Gets a s32 bits hash value from 1 to 4 integers, a float or a string.
		static unsigned int get32(int i);
		static unsigned int get32(int i, int j);
		static unsigned int get32(int i, int j, int k);
		static unsigned int get32(int i, int j, int k, int l);

		// Gets a s32 bits hash value from 1 to 4 floats.
		static unsigned int get32(float x);
		static unsigned int get32(float x, float y);
		static unsigned int get32(float x, float y, float z);
		static unsigned int get32(float x, float y, float z, float w);

		// Gets a s32 bits hash value from a string or a buffer.
		static unsigned int get32(const unsigned char* str);
		static unsigned int get32(const char* str);
		static unsigned int get32(const void* ptr, int size);

		// Gets a s32 bits hash value from an object.
		template<typename T>
		static unsigned int get32(const T& x)
		{
			return get32(static_cast<const void*>(&x), sizeof(T));
		}

		template<typename T>
		static float get32f(const T& x)
		{
			return float(get32(static_cast<const void*>(&x), sizeof(T))) / float(0xffffffff);
		}
	};
}

#endif // HASH_HH
