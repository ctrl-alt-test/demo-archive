#include "Gradient.hh"

#include "Hash.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/msys_temp.hh"


using namespace Algebra;
using namespace Noise;

//
// FIXME: tous ces tableaux sont probablement pas très efficaces en
// taille de code généré. Si ça pose un problème, ça peut se faire
// mieux sans trop d'effort.
//

static const float gradients1D[] = { 1.f, -1.f };
static const int mask1D = ARRAY_LEN(gradients1D);

static vector2f gradients2D[] = {
	{  1.f,  0.f },
	{ -1.f,  0.f },
	{  0.f,  1.f },
	{  0.f, -1.f },
	{  1.f,  1.f },
	{ -1.f,  1.f },
	{  1.f,  1.f },
	{  1.f, -1.f },
};

static const int mask2D = ARRAY_LEN(gradients2D);


static vector3f gradients3DInCube[] = {
	{  1.f,  1.f,  0.f },
	{ -1.f,  1.f,  0.f },
	{  1.f, -1.f,  0.f },
	{ -1.f, -1.f,  0.f },
	{  1.f,  0.f,  1.f },
	{ -1.f,  0.f,  1.f },
	{  1.f,  0.f, -1.f },
	{ -1.f,  0.f, -1.f },
	{  0.f,  1.f,  1.f },
	{  0.f, -1.f,  1.f },
	{  0.f,  1.f, -1.f },
	{  0.f, -1.f, -1.f },

	{  1.f,  1.f,  0.f },
	{ -1.f,  1.f,  0.f },
	{  0.f, -1.f,  1.f },
	{  0.f, -1.f, -1.f },
};
static const int mask3DCube = ARRAY_LEN(gradients3DInCube);


// FIXME : pourquoi les gradients du simplex sont normalisés mais pas
//         les autres ?
static vector3f gradients3DInSimplex[] = {
	{  1.f,  1.f,  0.f },
	{ -1.f,  1.f,  0.f },
	{  1.f, -1.f,  0.f },
	{ -1.f, -1.f,  0.f },
	{  1.f,  0.f,  1.f },
	{ -1.f,  0.f,  1.f },
	{  1.f,  0.f, -1.f },
	{ -1.f,  0.f, -1.f },
	{  0.f,  1.f,  1.f },
	{  0.f, -1.f,  1.f },
	{  0.f,  1.f, -1.f },
	{  0.f, -1.f, -1.f },

	{  1.f,  1.f,  0.f },
	{ -1.f,  1.f,  0.f },
	{  1.f, -1.f,  0.f },
	{ -1.f, -1.f,  0.f },
	{  1.f,  0.f,  1.f },
	{ -1.f,  0.f,  1.f },
	{  1.f,  0.f, -1.f },
	{ -1.f,  0.f, -1.f },
	{  0.f,  1.f,  1.f },
	{  0.f, -1.f,  1.f },
	{  0.f,  1.f, -1.f },
	{  0.f, -1.f, -1.f },

	{  1.f,  1.f,  1.f },
	{ -1.f,  1.f,  1.f },
	{  1.f, -1.f,  1.f },
	{ -1.f, -1.f,  1.f },
	{  1.f,  1.f, -1.f },
	{ -1.f,  1.f, -1.f },
	{  1.f, -1.f, -1.f },
	{ -1.f, -1.f, -1.f },
};
static const int mask3DSimplex = ARRAY_LEN(gradients3DInSimplex);

static vector4f gradients4D[] = {
	{  1.f,  1.f,  1.f,  0.f },
	{  1.f,  1.f,  0.f,  1.f },
	{  1.f,  0.f,  1.f,  1.f },
	{  0.f,  1.f,  1.f,  1.f },

	{  1.f,  1.f, -1.f,  0.f },
	{  1.f,  1.f,  0.f, -1.f },
	{  1.f,  0.f,  1.f, -1.f },
	{  0.f,  1.f,  1.f, -1.f },

	{  1.f, -1.f,  1.f,  0.f },
	{  1.f, -1.f,  0.f,  1.f },
	{  1.f,  0.f, -1.f,  1.f },
	{  0.f,  1.f, -1.f,  1.f },

	{  1.f, -1.f, -1.f,  0.f },
	{  1.f, -1.f,  0.f, -1.f },
	{  1.f,  0.f, -1.f, -1.f },
	{  0.f,  1.f, -1.f, -1.f },

	{ -1.f,  1.f,  1.f,  0.f },
	{ -1.f,  1.f,  0.f,  1.f },
	{ -1.f,  0.f,  1.f,  1.f },
	{  0.f, -1.f,  1.f,  1.f },

	{ -1.f,  1.f, -1.f,  0.f },
	{ -1.f,  1.f,  0.f, -1.f },
	{ -1.f,  0.f,  1.f, -1.f },
	{  0.f, -1.f,  1.f, -1.f },

	{ -1.f, -1.f,  1.f,  0.f },
	{ -1.f, -1.f,  0.f,  1.f },
	{ -1.f,  0.f, -1.f,  1.f },
	{  0.f, -1.f, -1.f,  1.f },

	{ -1.f, -1.f, -1.f,  0.f },
	{ -1.f, -1.f,  0.f, -1.f },
	{ -1.f,  0.f, -1.f, -1.f },
	{  0.f, -1.f, -1.f, -1.f },
};
static const int mask4D = ARRAY_LEN(gradients4D);

void Gradient::init()
{
	for (size_t i = 0; i < ARRAY_LEN(gradients2D); ++i)
	{
		normalize(gradients2D[i]);
	}

	for (size_t i = 0; i < ARRAY_LEN(gradients3DInSimplex); ++i)
	{
		normalize(gradients3DInSimplex[i]);
	}
}

float Gradient::get(int i)
{
	return gradients1D[Hash::get8(i) % mask1D];
}

vector2f Gradient::get(int i, int j)
{
	return gradients2D[Hash::get8(i, j) % mask2D];
}

vector3f Gradient::getInCube(int i, int j, int k)
{
	return gradients3DInCube[Hash::get8(i, j, k) % mask3DCube];
}

vector3f Gradient::getInSimplex(int i, int j, int k)
{
	return gradients3DInSimplex[Hash::get8(i, j, k) % mask3DSimplex];
}

vector4f Gradient::get(int i, int j, int k, int l)
{
	return gradients4D[Hash::get8(i, j, k, l) % mask4D];
}
