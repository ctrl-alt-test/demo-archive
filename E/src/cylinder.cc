//
// Forme de révolution
//

#include "cylinder.hh"
#include "vector.hh"

namespace Mesh
{
  // Cylindre
  float pipeHeightForAxial(float axial) { return axial; }
  float pipeRadiusForAxial(float axial) { return 1; }

  // Sphère
  float sphereHeightForAxial(float axial)
  {
    const float lat = PI * (axial - 0.5f);
    return 0.5f * msys_sinf(lat);
  }

  float sphereRadiusForAxial(float axial)
  {
    const float lat = PI * (axial - 0.5f);
    return msys_cosf(lat);
  }

  // Truc bizarre
  float trucHeightForAxial(float axial) { return axial; }
  float trucRadiusForAxial(float axial)
  {
    return 0.9f + 0.1f * msys_sinf(40.f * axial);
  }

  // Tore
  float torusHeightForAxial(float axial)
  {
    const float lat = 2.f * PI * (axial - 0.5f);
    return 0.5f * msys_sinf(lat);
  }

  float torusRadiusForAxial(float axial)
  {
    const float lat = 2.f * PI * (axial - 0.5f);
    return 1.f + 0.5f * msys_cosf(lat);
  }

  // Pneu
  float tireHeightForAxial(float axial)
  {
    const float lat = 2.f * PI * (axial - 0.5f);
    return 0.5f * msys_sinf(lat);
  }

  float tireRadiusForAxial(float axial)
  {
    const float lat = 2.f * PI * (axial - 0.5f);
    const float cos = msys_cosf(lat);
    const float abs = 0.0001f + msys_fabsf(cos);
    const float sign = 1.f - 2.f * (cos < 0);
    return 1.f + 0.35f * sign * msys_powf(abs, 0.5f);
  }

  // Colonne
  float columnHeightForAxial(float axial)
  {
    return axial;
  }

  float columnRadiusForAxial(float axial)
  {
    const float sin = msys_sinf(2 * PI * 20.f * axial);
    return 1.f - 0.2f * (sin > 0 ? msys_powf(sin, 0.5f) : 0);
  }



  // Calcul de normales
  static vector3f computeNormal(const vector3f OA,
				const vector3f OB,
				const vector3f OC,
				const vector3f OD)
  {
    const vector3f n1 = OB ^ OA;
    const vector3f n2 = OC ^ OB;
    const vector3f n3 = OD ^ OC;
    const vector3f n4 = OA ^ OD;

    vector3f n = n1 + n2 + n3 + n4;
    normalize(n);

    return n;
  }

  vector3f computeNormal(const Array<vertex> & uniqueVertices,
			 int radialFaces,
			 int i0, int i, int i2,
			 int j0, int j, int j2)
  {
    const vertex & _O = uniqueVertices[i  + j  * (radialFaces + 1)];

    const vertex & _A = uniqueVertices[i2 + j  * (radialFaces + 1)];
    const vertex & _B = uniqueVertices[i  + j2 * (radialFaces + 1)];
    const vertex & _C = uniqueVertices[i0 + j  * (radialFaces + 1)];
    const vertex & _D = uniqueVertices[i  + j0 * (radialFaces + 1)];

    const vector3f O = {_O.x, _O.y, _O.z};
    const vector3f A = {_A.x, _A.y, _A.z};
    const vector3f B = {_B.x, _B.y, _B.z};
    const vector3f C = {_C.x, _C.y, _C.z};
    const vector3f D = {_D.x, _D.y, _D.z};

    const vector3f OA = A - O;
    const vector3f OB = B - O;
    const vector3f OC = C - O;
    const vector3f OD = D - O;

    vector3f n = computeNormal(OA, OB, OC, OD);
    normalize(n);

    return n;
  }

  vector3f generalPurposeComputeNormal(const Array<vertex> & uniqueVertices,
				       int radialFaces, int axialFaces,
				       int i, int j)
  {
    int j0 = j - 1;
    int j2 = j + 1;
    if (j0 < 0)
      j0 = 0;
    if (j2 > axialFaces)
      j2 = axialFaces;

    int i0 = i - 1;
    int i2 = i + 1;
    if (i0 < 0)
      i0 = radialFaces - 1;
    if (i2 > radialFaces)
      i2 = 1;

    return computeNormal(uniqueVertices, radialFaces, i0, i, i2, j0, j, j2);
  }

  vector3f sphereComputeNormal(const Array<vertex> & uniqueVertices,
			       int radialFaces, int axialFaces,
			       int i, int j)
  {
    const vertex & _O = uniqueVertices[i  + j  * (radialFaces + 1)];
    vector3f n = {_O.x, _O.y, _O.z};
    normalize(n);

    return n;
  }

  vector3f generalPurposeComputeNormalWithBootoms(const Array<vertex> & uniqueVertices,
						  int radialFaces, int axialFaces,
						  int i, int j)
  {
    vector3f n = {0, 0, 0};
    if (0 == j)
    {
      n.y = -1.f;
    }
    else if (axialFaces == j)
    {
      n.y = 1.f;
    }
    else
    {
      n = generalPurposeComputeNormal(uniqueVertices,
				      radialFaces, axialFaces,
				      i, j);
    }
    return n;
  }

  vector3f torusComputeNormal(const Array<vertex> & uniqueVertices,
			      int radialFaces, int axialFaces,
			      int i, int j)
  {
    int j0 = j - 1;
    int j2 = j + 1;
    if (j0 < 0)
      j0 = axialFaces - 1;
    if (j2 > axialFaces)
      j2 = 1;

    int i0 = i - 1;
    int i2 = i + 1;
    if (i0 < 0)
      i0 = radialFaces - 1;
    if (i2 > radialFaces)
      i2 = 1;

    return computeNormal(uniqueVertices, radialFaces, i0, i, i2, j0, j, j2);
  }

  //
  // Etapes
  //
  // - avoir un cylindre correct
  // - avoir une fonction axiale
  // - tester avec une sphère
  // - avoir deux fonction radiale
  // - tester avec un pavé
  // - avoir une orientation radiale
  // - tester avec un truc bizarre
  // - brancher avec un LSystem
  //

  Array<vertex> * mesh1 = NULL;
  Array<vertex> * mesh2 = NULL;
  Array<vertex> * mesh3 = NULL;
  Array<vertex> * mesh4 = NULL;
  Array<vertex> * mesh5 = NULL;
  Array<vertex> * mesh6 = NULL;
  Array<vertex> * mesh7 = NULL;

  void generateTestMeshes()
  {
    // Prisme à trois pans
    mesh1 = new Array<vertex>();
    addCylinder(*mesh1, pipeRadiusForAxial, pipeHeightForAxial,
		generalPurposeComputeNormal,
		2.f, 5.f, 3, 1);

    // Cylindre de base
    mesh2 = new Array<vertex>();
    addCylinder(*mesh2, pipeRadiusForAxial, pipeHeightForAxial,
		generalPurposeComputeNormal,
		2.f, 5.f, 24, 3);

    // Sphère
    mesh3 = new Array<vertex>();
    addCylinder(*mesh3, sphereRadiusForAxial, sphereHeightForAxial,
		sphereComputeNormal,
		2.f, 2.f, 24, 10);

    // Truc
    mesh4 = new Array<vertex>();
    addCylinder(*mesh4, trucRadiusForAxial, trucHeightForAxial,
		generalPurposeComputeNormal,
// 		2.f, 10.f, 24, 100);
		2.f, 10.f, 12, 20);

    // Tore
    mesh5 = new Array<vertex>();
    addCylinder(*mesh5, torusRadiusForAxial, torusHeightForAxial,
		torusComputeNormal,
// 		2.f, 1.f, 48, 48);
		2.f, 1.f, 12, 12);

  // Pneu
    mesh6 = new Array<vertex>();
    addCylinder(*mesh6, tireRadiusForAxial, tireHeightForAxial,
		torusComputeNormal,
// 		4.f, 2.f, 48, 48);
		4.f, 2.f, 12, 12);

    // Column
    mesh7 = new Array<vertex>();
    addCylinder(*mesh7, columnRadiusForAxial, columnHeightForAxial,
		generalPurposeComputeNormal,
// 		2.f, 10.f, 48, 100);
		2.f, 10.f, 12, 10);
  }


  static vector3f computeTangent(const vector3f n,
				 const vector3f OA)
  {
    const vector3f bt = n ^ OA;

    vector3f t = bt ^ n;
    normalize(t);

    return t;
  }

  void addCylinder(Array<vertex> & vertices,
		   float (*radiusForAxial)(float axial),
		   float (*heightForAxial)(float axial),
		   vector3f (*computeNormal)(const Array<vertex> & uniqueVertices,
					     int radialFaces, int axialFaces,
					     int i, int j),
		   float diameter,
		   float length,
		   int radialFaces,
		   int axialFaces)
  {
    assert(radialFaces >= 3);
    assert(axialFaces >= 1);

    const unsigned int numberOfUniqueVertices = (axialFaces + 1) * (radialFaces + 1);
    Array<vertex> uniqueVertices(numberOfUniqueVertices);

    // Passe de calcul des sommets
    for (int j = 0; j <= axialFaces; ++j)
    {
      const float axial = float(j) / axialFaces;
      const float y = length * heightForAxial(axial);
      const float l = diameter * 0.5f * radiusForAxial(axial);

      for (int i = 0; i <= radialFaces; ++i)
      {
	const float radial = float(i) / radialFaces;

	const float lon = 2.f * PI * radial;
	const float x = l * msys_cosf(lon);
	const float z = l * msys_sinf(lon);

	const float q = 0.0001f + 0.9999f * radiusForAxial(axial);

	const vertex p = {x, y, z, // Position
			  0, 0, 0, // Normale (plus tard)
			  0, 0, 0, // Tangente (plus tard)
			  1.f, 1.f, 1.f, // Couleur
			  q * axial, q * radial, 0, q}; // Coordonnée de texture

	uniqueVertices.add(p);
      }
    }

    // Passe de calcul des normales
    for (int j = 0; j <= axialFaces; ++j)
      for (int i = 0; i <= radialFaces; ++i)
      {
	vertex & p = uniqueVertices[i + j * (radialFaces + 1)];

	const int i2 = (i == radialFaces ? 1 : i + 1);
	const vertex & _A = uniqueVertices[i2 + j  * (radialFaces + 1)];
	const vector3f O = {p.x, p.y, p.z};
	const vector3f A = {_A.x, _A.y, _A.z};
	const vector3f OA = A - O;

	const vector3f n = computeNormal(uniqueVertices,
					 radialFaces, axialFaces,
					 i, j);
	const vector3f t = computeTangent(n, OA);

	p.nx = n.x;
	p.ny = n.y;
	p.nz = n.z;

	p.tx = t.x;
	p.ty = t.y;
	p.tz = t.z;
      }

    // Assemblage en primitives
    const unsigned int supposedNumberOfVertices = 4 * radialFaces * axialFaces;
    vertices.init(supposedNumberOfVertices);

    for (int j = 0; j < axialFaces; ++j)
      for (int i = 0; i < radialFaces; ++i)
      {
	vertices.add(uniqueVertices[ i      +  j      * (radialFaces + 1)]);
	vertices.add(uniqueVertices[ i      + (j + 1) * (radialFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) + (j + 1) * (radialFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) +  j      * (radialFaces + 1)]);
      }
  }
}
