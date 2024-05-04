#ifndef RAND_HH
#define RAND_HH

namespace Noise
{
	/// <summary>
	/// Random value generator.
	/// </summary>
	class Rand
	{
	public:
		Rand(int seed = 0): _seed(seed) {}

		/// <summary>
		/// Integer random number.
		/// </summary>
		int   igen();

		/// <summary>
		/// Unsigned float random number: [0 .. 1].
		/// </summary>
		float fgen() { return (float)igen() / 32767.f; }

		/// <summary>
		/// Signed float random number: [-1 .. 1].
		/// </summary>
		float sfgen() { return (2.f * (float)igen() / 32767.f) - 1.f; }

		/// <summary>
		/// Integer random number: [0 .. n - 1].
		/// </summary>
		int   igen(int n) { return igen() % n; }

		/// <summary>
		/// Integer within [a .. b].
		/// </summary>
		int   igen(int a, int b) { return a + igen() % (b - a + 1); }

		/// <summary>
		/// Float within [a .. b].
		/// </summary>
		float fgen(float a, float b) { return a + (b - a) * fgen(); }

		/// <summary>
		/// Float within [average - spread .. average + spread].
		/// </summary>
		float fgenOnAverage(float average, float spread)
		{
			return fgen(average - spread, average + spread);
		}

		/// <summary>
		/// Boolean with probaTrue chance of being true.
		/// </summary>
		bool boolean(float probaTrue) { return fgen(0.f, 1.f) < probaTrue; }

		int seed() const { return _seed; }

		int _seed;
	};
}

#endif // RAND_HH
