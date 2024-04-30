//
// Forme de révolution
//

#include "revolution.hh"

#include "algebra/matrix.hh"
#include "algebra/vector2.hxx"
#include "algebra/vector3.hxx"
#include "interpolation.hh"
#include "mesh/mesh.hh"
#include "mesh/vertex.hh"
#include "spline.hh"

using namespace Mesh;

//
// Etapes
//
// - avoir une orientation radiale
// - brancher avec un LSystem
//

vector3f Revolution::_splineHFunc(float t, float theta)
{
  vector2f vec = _spline->get2(t);
  return vector3f(0.f, vec.x, 0.f);
}

float Revolution::_splineRFunc(float t, float theta)
{
  vector2f vec = _spline->get2(t);
  return vec.y + 0.000001f;
  // Le epsilon sert à éviter d'avoir un rayon nul,
  // qui pose problème pour les tangentes
}

vector2f Revolution::_revolve(float t, float thetaLevel)
{
  const float theta = 2.f * PI * thetaLevel;
  const float radius = (_rFunc ? _rFunc(t, theta) : _splineRFunc(t, theta));

  return radius * vector2f(msys_cosf(theta), msys_sinf(theta));
}

static vector3f computeTangent(const vector3f & n,
			       const vector3f & OA)
{
  const vector3f bt = cross(n, OA);

  vector3f t = cross(bt, n);
  normalize(t);

  return t;
}

vector3f Revolution::_getCenterPosition(float t, float thetaLevel)
{
  if (_hFunc)
    return _hFunc(t, thetaLevel);
  return _splineHFunc(t, thetaLevel);
}

// Texture enroulée autour de la forme
inline
static vector3f _computeTexCoord(float t, float thetaLevel, float radius)
{
  return vector3f(t, thetaLevel, radius == 0 ? 1.f : radius);
}

void Revolution::_computePositions(Array<vertex> & vertices,
				   float hScale, float rScale,
				   int tFaces, int thetaFaces)
{
  //
  // FIXME : si au lieu de mettre le code dans la boucle, on génère
  // une liste de points pour la passer à une fonction, on peut
  // appeler cette fonction avec une liste de point préparée, pour
  // avoir des arrêtes, etc.
  //

  vector3f X(1.f, 0, 0);
  vector3f Y(0, 1.f, 0);

  int thetaStrongEdgePass = 0;
  if (thetaFaces < 0)
  {
    thetaFaces = -thetaFaces;
    thetaStrongEdgePass = 1;
  }

  for (int j = 0; j <= tFaces; ++j)
  {
    const float t = float(j) / tFaces;

    const vector3f p1 = (_getCenterPosition(float(j) / float(tFaces), 0) +
			 _getCenterPosition(float(j) / float(tFaces), 0.5f * PI) +
			 _getCenterPosition(float(j) / float(tFaces), PI) +
			 _getCenterPosition(float(j) / float(tFaces), 1.5f * PI));
    const vector3f p2 = (_getCenterPosition((float(j) + 0.5f) / float(tFaces), 0) +
			 _getCenterPosition((float(j) + 0.5f) / float(tFaces), 0.5f * PI) +
			 _getCenterPosition((float(j) + 0.5f) / float(tFaces), PI) +
			 _getCenterPosition((float(j) + 0.5f) / float(tFaces), 1.5f * PI));

    vector3f dp = p2 - p1;

    if (dot(dp, Y) < 0) { dp = -dp; }
    if (norm(dp) > 0.0001f)
    {
      normalize(dp);
      Y = dp;
    }

    vector3f Z = cross((_absoluteOrientation ? vector3f(1.f, 0, 0) : X), Y);
    X = cross(Y, Z);
    matrix4 m = matrix4::orthonormalBasis(X, Y, Z);


    for (int i = 0; i <= thetaFaces; ++i)
    {
      for (int thetaEdge = 0; thetaEdge == 0 || (thetaEdge <= thetaStrongEdgePass && i < thetaFaces); ++thetaEdge)
      {
	const float thetaLevel = float(i) / thetaFaces + 0.0001f * float(thetaEdge);

	vector2f section = (_secFunc ? _secFunc(t, thetaLevel) : _revolve(t, thetaLevel));
	vector3f revolutionCenter = _getCenterPosition(t, thetaLevel);

	vector3f nPos = vector3f(section.x, revolutionCenter.y, section.y);
	vector3f pos = vector3f(rScale * section.x, 0, rScale * section.y);

	applyMatrixToVector(m, pos);

	// translation
	pos += revolutionCenter * hScale;

#if VERTEX_ATTR_TEX_COORD
	const vector3f texCoord = _computeTexCoord(t, thetaLevel, 1.f);
#endif

	const vertex p = {
#if VERTEX_ATTR_POSITION
	  pos, // Position
#endif
#if VERTEX_ATTR_NORMAL
	  vector3f(0, 0, 0), // Normale (plus tard)
#endif
#if VERTEX_ATTR_TANGENT
	  vector3f(0, 0, 0), // Tangente (plus tard)
#endif
#if VERTEX_ATTR_COLOR
	  1.f, 1.f, 1.f, // Couleur
#endif
#if VERTEX_ATTR_TEX_COORD
	  texCoord.x, texCoord.y, 0, texCoord.z, // Coordonnée de texture
#endif
#if VERTEX_ATTR_BARY_COORD
	  0, 0, 0, 0, // Coordonnées barycentriques (plus tard)
#endif
#if VERTEX_ATTR_ID
	  0,
#endif
	};

	vertices.add(p);
      }
    }
  }
}

void Revolution::_computeNormals(Array<vertex> & vertices,
				 int tFaces, int thetaFaces)
{
#if VERTEX_ATTR_POSITION
  for (int j = 0; j <= tFaces; ++j)
    for (int i = 0; i <= thetaFaces; ++i)
    {
      vertex & p = vertices[j * (thetaFaces + 1) + i];

      const int i2 = (i == thetaFaces ? 1 : i + 1);
      const vertex & _A = vertices[i2 + j  * (thetaFaces + 1)];
      const vector3f & O = p.p;
      const vector3f & A = _A.p;
      const vector3f OA = A - O;

#if VERTEX_ATTR_NORMAL
      p.n = _computeNormalFunc(vertices, tFaces, thetaFaces, i, j);
#if VERTEX_ATTR_TANGENT
      p.t = computeTangent(p.n, OA);
#endif
#endif
    }
#endif
}

void Revolution::_makeQuadsFromVertices(Array<vertex> & uniqueVertices,
					Array<vertex> & vertices,
					int tFaces, int thetaFaces)
{
  for (int j = 0; j < tFaces; ++j)
    for (int i = 0; i < thetaFaces; ++i)
    {
      vertex A = uniqueVertices[ i      +  j      * (thetaFaces + 1)];
      vertex B = uniqueVertices[ i      + (j + 1) * (thetaFaces + 1)];
      vertex C = uniqueVertices[(i + 1) + (j + 1) * (thetaFaces + 1)];
      vertex D = uniqueVertices[(i + 1) +  j      * (thetaFaces + 1)];

#if VERTEX_ATTR_BARY_COORD
      A.d0 = 1; A.d1 = 1; A.d2 = 0; A.d3 = 0;
      B.d0 = 0; B.d1 = 1; B.d2 = 1; B.d3 = 0;
      C.d0 = 0; C.d1 = 0; C.d2 = 1; C.d3 = 1;
      D.d0 = 1; D.d1 = 0; D.d2 = 0; D.d3 = 1;
#endif

      vertices.add(A);
      vertices.add(B);
      vertices.add(C);
      vertices.add(D);
    }
}

void Revolution::generateMesh(MeshStruct & mesh,
			      float hScale, float rScale,
			      int tFaces, int thetaFaces)
{
  int signedThetaFaces = thetaFaces;
  if (thetaFaces < 0)
  {
    thetaFaces = -2 * thetaFaces;
  }

  assert(tFaces >= 1);
  assert(thetaFaces >= 3);

  Array<vertex> uniqueVertices((tFaces + 1) * (thetaFaces + 1));
  IFDBG(assert(mesh.vertices.max_size - mesh.vertices.size >= 4 * tFaces * thetaFaces));

  _computePositions(uniqueVertices, hScale, rScale, tFaces, signedThetaFaces);
  _computeNormals(uniqueVertices, tFaces, thetaFaces);
  _makeQuadsFromVertices(uniqueVertices, mesh.vertices, tFaces, thetaFaces);
}
