
#include "engine/container/Algorithm.hxx"
#include "engine/container/Array.hxx"
#include "engine/container/Dico.hxx"
#include "engine/container/HashTable.hxx"
#include "engine/noise/Hash.hh"
#include "engine/noise/Rand.hh"
//#include "sys/msys_debug.h"
//#include "sys/msys_timer.h"

using namespace Container;

const char* strs[] = {
  "algorithm.hxx", "array.hxx", "engine/noise/hash.hh",
  "engine/noise/randomness.hh", "sys/msys_debug.h", "sys/msys_timer.h",

  "tex0", "tex1", "tex2", "tex3", "tex4", "tex5", "tex6", "tex7",
  "ambient", "lightCol[0]", "lightPos[0]", "lightAtt[0]",
  "lightCol[1]", "lightPos[1]", "lightAtt[1]", "retroEdgeThick",
  "retroFaceColor", "retroEdgeColor", "retroParams", "lensFishEye",
  "lensGlowPow", "lensStreakPow", "lensOrbsSize", "lensOrbsCol",
  "lensFlareCol", "lensGlowCol", "lensStreakCol", "focus", "skip",
  "vignetting", "grainParams", "fade", "fadeLuminance",
  "resolution", "invResolution", "center", "pass", "cameraMatrix",
  "zNear", "zFar", "time", "yPos", "oldYPos", "skyCol", "fogCol",
  "fogDensity", "id", "birthDate", "shininess", "trans",
  "oldModelView", "textStep",

  "stdlib.vert", "stdlib.frag", "lightFunctions.frag",
  "screenspace/blur.frag", "screenspace/postEffects.frag",
  "minimal.vert", "minimal.frag", "createVSM.vert", "createVSM.frag",
  "bump.vert", "bump.frag", "text3D.vert", "particles.vert",
  "particles.frag", "retro.frag", "danceFloor.frag", "heightmap.vert",
  "hillsDisplacement.vert", "popping.vert", "growTest.vert",
  "lightSource.frag", "anisotropic.frag", "getAlbedo.frag",
  "getAmbient_envmap.frag", "attractor.frag", "godrays.frag",
  "screenspace/fxaa.frag", "screenspace/postProcessing.vert",
  "screenspace/noPostProcessing.frag",
  "screenspace/horizontalBlurPass.frag",
  "screenspace/verticalBlurPass.frag",
  "screenspace/horizontalStreakPass.frag",
  "screenspace/verticalStreakPass.frag",
  "screenspace/bokehFirstPass.frag",
  "screenspace/bokehSecondPass.frag", "screenspace/sky.vert",
  "screenspace/sky.frag", "screenspace/finalPass.frag",
  "screenspace/antialiasPass.frag", "screenspace/lensOrbs.vert",
  "screenspace/lensOrbs.frag", "text.vert", "text.frag",
  "debug/debugNonLinear.frag", "debug/debugNormals.vert",
  "debug/debugNormals.frag", "debug/debugLight.vert",
  "debug/debugLight.frag", "debug/debugWhiteLight.frag",
  "debug/debugZBuffer.frag",
};

void checkHash()
{
  Hash::init();
  {
    int count[256];
    for (int i = 0; i < 256; ++i) { count[i] = 0; }
    for (int i = 0; i < 256; ++i) { ++count[Hash::get8(i)]; }
    for (int i = 0; i < 256; ++i) { assert(count[i] == 1); }
  }

  {
    int count[4096];
    for (int i = 0; i < 4096; ++i) { count[i] = 0; }
    for (int i = 0; i < 1000000; ++i)
    {
      int h = Hash::get32(i);
      ++count[h & 0xfff];
    }

    int min = 4096;
    int max = 0;
    int sum = 0;
    for (int i = 0; i < 4096; ++i)
    {
      sum += count[i];
      if (count[i] < min) { min = count[i]; }
      if (count[i] > max) { max = count[i]; }
    }
    DBG("Med: %.2f", float(sum)/4096.f);
    DBG("Min: %d", min);
    DBG("Max: %d", max);
  }

  for (int i = 0; i < sizeof(strs) / sizeof(strs[0]); ++i)
  {
    DBG("%s\t-> %d", strs[i], Hash::get32(strs[i]) & 0xff);
  }
}

void checkHashTable()
{
  Hash::init();
  HashTable<float, int> a(100);

  a.add(0.f, 0);
  a.add(2.f, 2);
  a.add(4.f, 4);
  a.add(6.f, 6);
  a.add(8.f, 8);

  a.add(3.f, 30000000);
  a.add(3.1f, 31000000);
  a.add(3.14f, 31400000);
  a.add(3.141f, 31410000);
  a.add(3.1415f, 31415000);
  a.add(3.14159f, 31415900);
  a.add(3.141592f, 31415920);
  a.add(3.1415926f, 31415926);

  a.add(0.0000001f, 10000000);
  a.add(0.000001f, 1000000);
  a.add(0.00001f, 100000);
  a.add(0.0001f, 10000);
  a.add(0.001f, 1000);
  a.add(0.01f, 100);
  a.add(0.1f, 10);
  a.add(1.f, 1);
  a.add(10.f, 10);
  a.add(100.f, 100);
  a.add(1000.f, 1000);
  a.add(10000.f, 10000);
  a.add(100000.f, 100000);
  a.add(1000000.f, 1000000);
  a.add(10000000.f, 10000000);

  assert(a[-1.f] == NULL);
  assert(*(a[3.14f]) != *(a[3.141f]));

  for (int i = 0; i < a.cells.size; ++i)
  {
    if (!a.cells[i].empty)
    {
      const float k = a.cells[i].key;
      const int v = a.values[i];
      const int* found = a[k];
      assert(found != NULL);
      assert(*found == v);
    }
  }
}


void checkDico()
{
  Dico<float, int> a(100);

  a.add(0.f, 0);
  a.add(2.f, 2);
  a.add(4.f, 4);
  a.add(6.f, 6);
  a.add(8.f, 8);

  a.add(3.f, 30000000);
  a.add(3.1f, 31000000);
  a.add(3.14f, 31400000);
  a.add(3.141f, 31410000);
  a.add(3.1415f, 31415000);
  a.add(3.14159f, 31415900);
  a.add(3.141592f, 31415920);
  a.add(3.1415926f, 31415926);

  a.add(0.0000001f, 10000000);
  a.add(0.000001f, 1000000);
  a.add(0.00001f, 100000);
  a.add(0.0001f, 10000);
  a.add(0.001f, 1000);
  a.add(0.01f, 100);
  a.add(0.1f, 10);
  a.add(1.f, 1);
  a.add(10.f, 10);
  a.add(100.f, 100);
  a.add(1000.f, 1000);
  a.add(10000.f, 10000);
  a.add(100000.f, 100000);
  a.add(1000000.f, 1000000);
  a.add(10000000.f, 10000000);

  assert(a[-1.f] == NULL);
  assert(*(a[3.14f]) != *(a[3.141f]));

  for (int i = 0; i < a.keys.size; ++i)
  {
    const float k = a.keys[i];
    const int v = a.values[i];
    const int* found = a[k];
    assert(found != NULL);
    assert(*found == v);
  }
}

void report(long startEvalTime, long endEvalTime, int size, int itemSize, int tests, const char* desc1, const char* desc2)
{
  float averageDuration = float(endEvalTime - startEvalTime) / float(tests);

  DBG("%s, %s, %d, %d, %.3f", desc1, desc2, size, itemSize, averageDuration);
}

void checkIsSorted()
{
  Array<int> a(1000);
  Rand r;

  a.empty();
  assert(Algorithm::isSorted(a) == true);

  a.add(1);
  assert(Algorithm::isSorted(a) == true);

  a.empty();
  for (int i = 0; i < 1000; ++i) a.add(0);
  assert(Algorithm::isSorted(a) == true);

  a.empty();
  for (int i = 0; i < 1000; ++i) a.add(i);
  assert(Algorithm::isSorted(a) == true);

  a.empty();
  for (int i = 0; i < 1000; ++i) a.add(1000 - i);
  assert(Algorithm::isSorted(a) == false);

  a.empty();
  for (int i = 0; i < 1000; ++i) a.add(i == 500 ? 1 : 2);
  assert(Algorithm::isSorted(a) == false);

  a.empty();
  for (int i = 0; i < 1000; ++i) a.add(r.igen());
  assert(Algorithm::isSorted(a) == false);
}

void checkBinarySearch(int size, Rand& r)
{
  Array<int> array(size);

  for (int i = 0; i < size; ++i) array.add(r.igen());
  Algorithm::quickSort(array);

  for (int i = 0; i < 2 * size; ++i)
  {
    const int search = r.igen();
    const int index = Algorithm::binarySearch(array, search);

    // Index cohérent ;
    // si on renvoie size, c'est que tout est plus petit que ce qu'on
    // cherche.
    assert(index >= 0);
    assert(index < array.size || array[array.size - 1] < search);

    // Tout ce qui est à gauche (si ça existe) est strictement plus petit ;
    // Tout ce qui est à droite (si ça existe) est plus grand ou égal.
    assert(index <= 0              || array[index - 1] < search);
    assert(index >= array.size - 1 || array[index + 1] >= search);
  }
}

void checkBinarySearch()
{
  Rand r;

  for (int i = 1; i < 1000; ++i)
  {
    checkBinarySearch(i, r);
  }
}


template<int N>
struct Item
{
  int value;
  char fill[1 + N];

  Item(int i): value(i) {}

  bool operator > (const Item<N>& rhs) const { return value > rhs.value; }
};

typedef void (sortFunction)(Array<int>&);


template<int N>
void checkSort(int arraySize, int tests, void (sort)(Array<Item<N> >&), const char* desc)
{
  long startEvalTime;
  Array<Item<N> > a(arraySize);

  startEvalTime = msys_timerGet();
  for (int j = 0; j < tests; ++j)
  {
    // Tableau avec la même valeur
    a.empty();
    for (int i = 0; i < arraySize; ++i) a.add(0);
    sort(a);
    assert(Algorithm::isSorted(a) == true);
  }
  report(startEvalTime, msys_timerGet(), arraySize, sizeof(Item<N>), tests, desc, "flat");


  startEvalTime = msys_timerGet();
  for (int j = 0; j < tests; ++j)
  {
    // Tableau déjà trié
    a.empty();
    for (int i = 0; i < arraySize; ++i) a.add(i);
    sort(a);
    assert(Algorithm::isSorted(a) == true);
  }
  report(startEvalTime, msys_timerGet(), arraySize, sizeof(Item<N>), tests, desc, "sorted");


  startEvalTime = msys_timerGet();
  for (int j = 0; j < tests; ++j)
  {
    // Tableau trié dans l'ordre inverse
    a.empty();
    for (int i = 0; i < arraySize; ++i) a.add(arraySize - i);
    sort(a);
    assert(Algorithm::isSorted(a) == true);
  }
  report(startEvalTime, msys_timerGet(), arraySize, sizeof(Item<N>), tests, desc, "rev-sorted");

  Rand r;
  startEvalTime = msys_timerGet();
  for (int j = 0; j < tests; ++j)
  {
    // Tableau aléatoire
    a.empty();
    for (int i = 0; i < arraySize; ++i) a.add(r.igen());
    sort(a);
    assert(Algorithm::isSorted(a) == true);
  }
  report(startEvalTime, msys_timerGet(), arraySize, sizeof(Item<N>), tests, desc, "random");
}

template<int N>
void checkSorts()
{
  const int maxSlowTests = 5000;
  const int maxFastTests = 100000;
  const int magicSlowTestNumber = 2 * 4096;
  const int magicFastTestNumber = 32 * 4096;

  for (int i = 16; i < maxSlowTests; i *= 2)
    checkSort<N>(i, magicSlowTestNumber / i, Algorithm::bubbleSort, "bubble sort");

  for (int i = 16; i < maxSlowTests; i *= 2)
    checkSort<N>(i, magicSlowTestNumber / i, Algorithm::insertionSort, "insertion sort");

  for (int i = 16; i < maxFastTests; i *= 2)
    checkSort<N>(i, magicFastTestNumber / i, Algorithm::quickSort, "quick sort");

  for (int i = 16; i < maxFastTests; i *= 2)
    checkSort<N>(i, magicFastTestNumber / i, Algorithm::mergeSort, "merge sort");
}

void testAlgorithms()
{
  checkHash();
  checkHashTable();
  checkDico();

  checkIsSorted();

#if 0
  checkSorts<0>();
  checkSorts<64>();
  checkSorts<256>();
  checkSorts<1024>();
#endif

  checkBinarySearch();

  assert(1 == 2);
}
