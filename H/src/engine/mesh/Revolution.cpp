#include "Revolution.hh"

#include "Mesh.hh"
#include "engine/algebra/Interpolation.hh"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Matrix.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"

using namespace Algebra;
using namespace mesh;

//
// Etapes
//
// - avoir une orientation radiale
// - brancher avec un LSystem
//

vector3f Revolution::_splineHFunc(float t, float)
{
	vector2f vec = _spline->get2(t);
	vector3f result = { 0.f, vec.x, 0.f };
	return result;
}

float Revolution::_splineRFunc(float t, float)
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

	vector2f direction = { msys_cosf(theta), msys_sinf(theta) };
	return radius * direction;
}

vector3f Revolution::_getCenterPosition(float t, float thetaLevel)
{
	if (_hFunc)
	{
		return _hFunc(t, thetaLevel);
	}
	return _splineHFunc(t, thetaLevel);
}

// Texture enroulée autour de la forme
inline
static vector3f _computeTexCoord(float thetaLevel, float t, float radius)
{
	// Angle on the u axis, t on the v axis.
	// But because of the circle path direction, we have to invert u.
	vector3f result = { 1.f - thetaLevel, t, radius == 0 ? 1.f : radius };
	return result;
}

void Revolution::ComputePositions(Container::Array<vertex>& vertices, int tFaces, int thetaFaces)
{
	//
	// FIXME : si au lieu de mettre le code dans la boucle, on génère
	// une liste de points pour la passer à une fonction, on peut
	// appeler cette fonction avec une liste de point préparée, pour
	// avoir des arrêtes, etc.
	//

	const vector3f absoluteX = { 1.f, 0, 0 };
	vector3f X = { 1.f, 0, 0 };
	vector3f Y = { 0, 1.f, 0 };

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

		if (dot(dp, Y) < 0)
		{
			dp = -dp;
		}
		if (norm(dp) > 0.0001f)
		{
			normalize(dp);
			Y = dp;
		}

		vector3f Z = cross((_absoluteOrientation ? absoluteX : X), Y);
		X = cross(Y, Z);
		matrix4 m = matrix4::orthonormalBasis(X, Y, Z);


		for (int i = 0; i <= thetaFaces; ++i)
		{
			for (int thetaEdge = 0; thetaEdge == 0 || (thetaEdge <= thetaStrongEdgePass && i < thetaFaces); ++thetaEdge)
			{
				const float thetaLevel = float(i) / thetaFaces + 0.0001f * float(thetaEdge);

				vector2f section = (_secFunc ? _secFunc(t, thetaLevel) : _revolve(t, thetaLevel));
				vector3f revolutionCenter = _getCenterPosition(t, thetaLevel);

				vector3f position = { section.x, 0, section.y };
				applyMatrixToDirection(m, position);

				// translation
				position += revolutionCenter;

#if VERTEX_ATTR_TEX_COORD
				const vector3f texCoord = _computeTexCoord(thetaLevel, t, 1.f);
#endif

				const vertex p = {
#if VERTEX_ATTR_POSITION
					position,
#endif
#if VERTEX_ATTR_NORMAL
					{ 0, 0, 0 }, // Normal will be computed later.
#endif
#if VERTEX_ATTR_TANGENT
					{ 0, 0, 0 }, 0.f, // Tangent will be computed later.
#endif
#if VERTEX_ATTR_COLOR
					1.f, 1.f, 1.f, // Color (deprecated).
#endif
#if VERTEX_ATTR_TEX_COORD
					texCoord.x, texCoord.y,
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

void Revolution::UpdateQuadsIndices(Mesh & mesh,
									int tFaces, int thetaFaces)
{
	for (int j = 0; j < tFaces; ++j)
	{
		for (int i = 0; i < thetaFaces; ++i)
		{
			int length = thetaFaces + 1;
			mesh.quads.add(i     + j     * length);
			mesh.quads.add(i     + (j+1) * length);
			mesh.quads.add((i+1) + (j+1) * length);
			mesh.quads.add((i+1) + j     * length);
		}
	}
}

void Revolution::GenerateMesh(Mesh & mesh, int tFaces, int thetaFaces)
{
	int signedThetaFaces = thetaFaces;
	if (thetaFaces < 0)
	{
		thetaFaces = -2 * thetaFaces;
	}

	assert(tFaces >= 1);
	assert(thetaFaces >= 3);

#if DEBUG
	assert(mesh.vertices.max_size - mesh.vertices.size >= 4 * tFaces * thetaFaces);
#endif

	ComputePositions(mesh.vertices, tFaces, signedThetaFaces);
	UpdateQuadsIndices(mesh, tFaces, thetaFaces);
}
