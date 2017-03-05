//
// Forme de révolution
//

#include "revolution.hh"

#include "interpolation.hh"
#include "spline.h"
#include "vector.hh"

#if DEBUG
#include <stdio.h>
#endif

namespace Mesh
{
  // --------------------------------------------------------------------------
  // Calcul de position des points

  // Cylindre débouché (in : version intérieure)
  vector3f pipeHFunc(float t, float theta) { return vector3f(0, t, 0); }
  vector3f inPipeHFunc(float t, float theta) { return vector3f(0, 1.f - t, 0); }
  float pipeRFunc(float t, float theta) { return 1.f; }

  // Roue
  vector3f wheelHFunc(float t, float theta)
  {
    if (t <= 0.25f ) return vector3f(0, -0.5f, 0);
    if (t >= 0.75f ) return vector3f(0, 0.5f, 0);
    const float alpha = PI * (2.f * t - 0.5f);
    return vector3f(0, -0.5f * msys_cosf(alpha), 0);
  }
  float wheelRFunc(float t, float theta)
  {
    if (t <= 0.25f) return 0.0001f + 3.2f * t;
    if (t >= 0.75f) return 0.0001f + 3.2f * (1.f - t);
    const float alpha = PI * (2.f * t - 0.5f);
    return 0.8f + 0.2f * msys_powf(msys_sinf(alpha), 0.7f);
  }

  // Sphère
  vector3f sphereHFunc(float t, float theta)
  {
    const float lat = PI * (t - 0.5f);
    return vector3f(0.f, msys_sinf(lat), 0.f);
  }
  float sphereRFunc(float t, float theta)
  {
    const float lat = PI * (t - 0.5f);
    return msys_cosf(lat) + 0.000001f;
  }

  // Hemisphère
  vector3f hemisphereHFunc(float t, float theta)
  {
    const float lat = 0.5f * PI * t;
    return vector3f(0.f, msys_sinf(lat), 0.f);
  }
  float hemisphereRFunc(float t, float theta)
  {
    const float lat = 0.5f * PI * t;
    return msys_cosf(lat) + 0.000001f;
  }

  // Tore
  vector3f torusHFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    return vector3f(0.f, 0.1f * msys_sinf(lat), 0.f);
  }
  float torusRFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    return 0.8f + 0.1f * msys_cosf(lat);
  }

#if 0

  // Truc bizarre
  vector3f trucHFunc(float t, float theta) { return vector3f(0.f, t, 0.f); }
  float trucRFunc(float t, float theta)
  {
    return 0.9f + 0.1f * msys_sinf(40.f * t);
  }

  // Pneu
  vector3f tireHFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    return vector3f(0.f, 0.5f * msys_sinf(lat), 0.f);
  }
  float tireRFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    const float cos = msys_cosf(lat);
    const float abs = 0.0001f + msys_fabsf(cos);
    const float sign = 1.f - 2.f * (cos < 0);
    return 1.f + 0.35f * sign * msys_powf(abs, 0.5f);
  }

  // Écrou
  vector3f ecrouHFunc(float t, float theta)
  {
    float y;
    if (6.f * t <= 3.f)
      y = 3.f * t;
    else if (6.f * t <= 4.f)
      y = 1.f;
    else if (6.f * t <= 5.f)
      y = 5.f - 6.f * t;
    else y = 0;
    return vector3f(0.f, y, 0.f);
  }
  float ecrouRFunc(float t, float theta)
  {
    return 1.f;
  }

  // Colonne
  vector3f columnHFunc(float t, float theta) { return vector3f(0.f, t, 0.f); }
  float columnRFunc(float t, float theta)
  {
    if (t < 0.05 || t > 0.95)
      return 1.f;
    if (t < 0.1 || t > 0.9)
      return 0.9f;

    const float sin = msys_sinf(10.f * theta);
    const float curve = 1.f - msys_powf((t - 0.1f)/0.4f - 1.f, 10.f);
    return 0.8f - 0.1f * curve * (sin > 0 ? msys_powf(sin, 0.2f) : 0);
  }

  // Square revolution
  float square(float theta)
  {
	float a = msys_fmodf((theta + PI / 4), PI / 2) - PI / 4;
	return 1.f / cos(a);
  }
#endif

  // --------------------------------------------------------------------------
  // Calcul de normales
  vector3f computeNormal(const vector3f & O,
			 const vector3f & A,
			 const vector3f & B,
			 const vector3f & C,
			 const vector3f & D)
  {
    const vector3f OA = A - O;
    const vector3f OB = B - O;
    const vector3f OC = C - O;
    const vector3f OD = D - O;

    const vector3f n1 = OB ^ OA;
    const vector3f n2 = OC ^ OB;
    const vector3f n3 = OD ^ OC;
    const vector3f n4 = OA ^ OD;

    vector3f n = n1 + n2 + n3 + n4;
    normalize(n);

    return n;
  }

  static
  vector3f computeNormalWithIndices(const Array<vertex> & vertices,
				    int strive,
				    int i0, int i1, int i2,
				    int j0, int j1, int j2)
  {
    const vector3f & _O = vertices[i1 + j1 * strive].p;
    const vector3f & _A = vertices[i2 + j1 * strive].p;
    const vector3f & _B = vertices[i1 + j2 * strive].p;
    const vector3f & _C = vertices[i0 + j1 * strive].p;
    const vector3f & _D = vertices[i1 + j0 * strive].p;

    return computeNormal(_O, _A, _B, _C, _D);
  }

  vector3f generalPurposeComputeNormal(const Array<vertex> & vertices,
				       int tFaces, int thetaFaces,
				       int i1, int j1)
  {
    const int j0 = max(j1 - 1, 0);
    const int j2 = min(j1 + 1, tFaces);

    const int i0 = (i1 - 1 < 0 ? thetaFaces - 1 : i1 - 1);
    const int i2 = (i1 + 1 > thetaFaces ? 1 : i1 + 1);

    return computeNormalWithIndices(vertices, thetaFaces + 1, i0, i1, i2, j0, j1, j2);

    /*
    //    i0 i1 i2
    // j2     B
    // j1  C  O  A
    // j0     D
    //
    const vector3f & _O = vertices[i1 + j1 * (thetaFaces + 1)].p;

    const vector3f & _A = vertices[i2 + j1 * (thetaFaces + 1)].p;
    const vector3f & _B = vertices[i1 + j2 * (thetaFaces + 1)].p;
    const vector3f & _C = vertices[i0 + j1 * (thetaFaces + 1)].p;
    const vector3f & _D = vertices[i1 + j0 * (thetaFaces + 1)].p;

    return computeNormal(_O, _A, _B, _C, _D);
    */
  }

  vector3f sphereComputeNormal(const Array<vertex> & uniqueVertices,
			       int tFaces, int thetaFaces,
			       int i, int j)
  {
    vector3f n = uniqueVertices[i  + j  * (thetaFaces + 1)].p;
    normalize(n);

    return n;
  }

  vector3f generalPurposeComputeNormalWithBottoms(const Array<vertex> & uniqueVertices,
						  int tFaces, int thetaFaces,
						  int i, int j)
  {
    vector3f n(0, 0, 0);
    if (0 == j)
    {
      n.y = -1.f;
    }
    else if (tFaces == j)
    {
      n.y = 1.f;
    }
    else
    {
      n = generalPurposeComputeNormal(uniqueVertices,
				      tFaces, thetaFaces,
				      i, j);
    }
    return n;
  }

  vector3f torusComputeNormal(const Array<vertex> & vertices,
			      int tFaces, int thetaFaces,
			      int i1, int j1)
  {
    const int j0 = (j1 - 1 < 0 ? tFaces - 1 : j1 - 1);
    const int j2 = (j1 + 1 > tFaces ? 1 : j1 + 1);

    const int i0 = (i1 - 1 < 0 ? thetaFaces - 1 : i1 - 1);
    const int i2 = (i1 + 1 > thetaFaces ? 1 : i1 + 1);

    return computeNormalWithIndices(vertices, thetaFaces + 1, i0, i1, i2, j0, j1, j2);
  }

#if DEBUG
  // Code pour charger un mesh de révolution à partir d'une spline dans un fichier
  static Array<float> splineData(1000);

  static vector3f splineHFunc(float t, float theta) {
    float ret[2];
    spline(splineData.elt, splineData.size / 3, 2, t, ret);
    return vector3f(0.f, ret[0] / 100.f + 0.001f, 0.f);
  }
  static float splineRFunc(float t, float theta) {
    float ret[2];
    spline(splineData.elt, splineData.size / 3, 2, t, ret);
    return ret[1] / 100.f + 0.001f;
  }

  Revolution loadSplineFromFile(const char *file)
  {
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];
    splineData.empty();
    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      int f[3];
      int result = sscanf(buffer, " %d , %d , %d ,", &f[0], &f[1], &f[2]);
      if (result == 3) {
        splineData.add((float) f[0]);
        splineData.add((float) f[1]);
        splineData.add((float) f[2]);
      }
    }

    // Calcul de t
    for (int i = 0; i < splineData.size; i += 3)
      splineData[i] = (float)i / (splineData.size - 3);

    return Revolution(splineHFunc, splineRFunc);
  }

#endif // DEBUG
}
