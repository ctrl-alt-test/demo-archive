#ifndef REVOLUTION_HH
#define REVOLUTION_HH

#include "RevolutionFunctions.hh"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/container/Array.hh"
#include "engine/timeline/Variable.hh"
#include <cassert>
#include <cstddef>

namespace mesh
{
	struct Mesh;
	struct vertex;

	class Revolution
	{
	public:
		typedef float (*fttheta)(float t, float thetaLevel);
		typedef Algebra::vector2f (*ftsection)(float t, float thetaLevel);
		typedef Algebra::vector3f (*fttheta_center)(float t, float thetaLevel);
		typedef Algebra::vector3f (*normalCompute)(const Container::Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);

		/// <summary/>
		///
		/// <param name="hFunc">Function returning the position of the axis point depending on t and theta.</param>
		/// <param name="rFunc">Function returning the radius depending on t and theta.</param>
		Revolution(fttheta_center hFunc, fttheta rFunc, ftsection secFunc = NULL, Timeline::Variable* var = NULL):
			_hFunc(hFunc),
			_rFunc(rFunc),
			_secFunc(secFunc),
			_spline(var),
			_absoluteOrientation(false)//absoluteOrientation)
		{
			assert(rFunc != NULL || secFunc != NULL || var != NULL);
			assert(hFunc != NULL || var != NULL);
		}

		/// <summary/>
		///
		/// <param name="tFaces">Number of faces along t.</param>
		/// <param name="thetaFaces">Number of faces along theta.</param>
		void				GenerateMesh(Mesh& mesh, int tFaces, int thetaFaces);

	private:
		Algebra::vector3f	_splineHFunc(float t, float theta);
		float				_splineRFunc(float t, float theta);
		Algebra::vector2f	_revolve(float t, float theta);

		Algebra::vector3f	_getCenterPosition(float t, float thetaLevel);

		void				ComputePositions(Container::Array<vertex>& vertices,
											 int tFaces, int thetaFaces);
		static void			UpdateQuadsIndices(Mesh& mesh,
											   int tFaces, int thetaFaces);

		fttheta_center		_hFunc;
		fttheta				_rFunc;
		ftsection			_secFunc;
		Timeline::Variable*	_spline;
		bool				_absoluteOrientation;
	};

#if DEBUG
	mesh::Revolution loadSplineFromFile(const char* file);
#endif

}

#endif // REVOLUTION_HH
