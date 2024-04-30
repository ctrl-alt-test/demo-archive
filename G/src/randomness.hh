//
// Random value generator
//

#ifndef		RANDOMNESS_HH
# define	RANDOMNESS_HH

class Rand
{
public:
  Rand(int seed = 0): _seed(seed) {}

  // Integer random number
  int   igen();

  // Unsigned float random number: [0 .. 1]
  float fgen() { return (float)igen() / 32767.f; }

  // Signed float random number: [-1 .. 1]
  float sfgen() { return (2.f * (float)igen() / 32767.f) - 1.f; }

  // Integer random number: [0 .. n - 1]
  int   igen(int n) { return igen() % n; }

  // Integer within [a .. b]
  int   igen(int a, int b) { return a + igen() % (b - a + 1); }

  // Float within [a .. b]
  float fgen(float a, float b) { return a + (b - a) * fgen(); }

  // Float within [average - spread .. average + spread]
  float fgenOnAverage(float average, float spread)
  {
    return fgen(average - spread, average + spread);
  }

  // Boolean with probaTrue chance of being true
  bool boolean(float probaTrue) { return fgen(0.f, 1.f) < probaTrue; }

  int seed() { return _seed; }

private:
  int _seed;
};

#endif		// RANDOMNESS_HH
