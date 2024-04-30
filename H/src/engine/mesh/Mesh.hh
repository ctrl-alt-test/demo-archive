//
// Définitions pour les meshs
//

#ifndef MESH_HH
#define MESH_HH

#include "engine/algebra/Vector2.hh"
#include "engine/container/Array.hh"
#include "engine/noise/Rand.hh"
#include "Vertex.hh"

namespace Algebra
{
	struct matrix4;
}

namespace Core
{
	struct RigidTransform;
}

namespace mesh
{
	typedef vertex (*MeshFunction)(int vertexId, int faceId, const vertex& inputVertex);

	/// <summary>
	/// A function that describes a volume, by returning the signed
	/// distance between a given point in space and the nearest point
	/// of the volume.
	/// The distance is positive if the point is outside the volume,
	/// or negative if the point is inside.
	/// </summary>
	typedef float (*SDFFunction)(float x, float y, float z);

	// A note on naming convention: this struct is still a work in
	// progress.
	// Methods with a lowercase first letter are old methods that haven't
	// been looked at yet and may or may not stay.
	// Methods with an uppercase first letter are here to stay.

	struct Mesh
	{
		Container::Array<int>		quads;
		Container::Array<vertex>	vertices;
		bool						shouldCleanUp;

		Mesh(): quads(), vertices(), shouldCleanUp(false) {}
		explicit Mesh(int capacity): quads(capacity), vertices(capacity), shouldCleanUp(false) {}

		void	Clear();
		Mesh& add(const Container::Array<vertex>& vertices);
		Mesh& add(const Mesh& mesh);
		Mesh& add(const Mesh& mesh, const Algebra::matrix4& transform);

		/// <summary>
		/// Delete unreferenced vertices and update indices.
		/// </summary>
		void	CleanUp();

		/// <summary>
		/// Cut off the parts of the mesh that are on the negative side
		/// of a signed distance field.
		/// </summary>
		Mesh&	Cut(SDFFunction sdf);

		/// <summary>
		/// Applies the function to each vertex of the mesh.
		/// </summary>
		Mesh&	ApplyFunction(MeshFunction function);
		Mesh&	ApplyFunction(const int* vertexIds, int numberOfVertices, MeshFunction function);

		/*
		// Pour le reste :
		typedef vertex *(combineOperator)(const vertex& lhs, const vertex& rhs);
		void combine(const Mesh& mesh, combineOperator op);
		*/

		Mesh&	Transform(const Algebra::matrix4& mat);
		Mesh&	Transform(const Core::RigidTransform& t);
		Mesh&	Translate(const Algebra::vector3f& t);
		Mesh&	Translate(float x, float y, float z);
		Mesh&	Rotate(float angle, const Algebra::vector3f& axis);
		Mesh&	Rotate(float angle, float x, float y, float z);
		Mesh&	Scale(const Algebra::vector3f& s);
		Mesh&	Scale(float s) { return Scale(s, s, s); }
		Mesh&	Scale(float x, float y, float z);

		void removeFace(int index);

		Algebra::vector3f faceCenter(int face) const;
		void transformFace(int face, const Algebra::matrix4& mat);

		/// <summary>
		/// Extrudes a face. If out_newFaces is not null, the ids of
		/// the four new faces are written to it.
		/// </summary>
		void	Extrude(int face, int* out_newFaces = nullptr);

		/// <summary>
		/// Turns the smooth edge between two faces into a crease.
		/// </summary>
		void	CreaseEdge(int face1, int face2);

		/// <summary>
		/// Turns the crease between two faces into a smooth edge.
		/// </summary>
		void	SmoothEdge(int face1, int face2);

		/// <summary>
		/// Computes the normals for all vertices.
		/// </summary>
		void	ComputeNormals();

		/// <summary>
		/// Computes the tangents for all vertices.
		/// </summary>
		void	ComputeTangents();

		void computeBaryCoord();
		void unsharpenEdges(float epsilon);
		void sharpenEdges();
		void smooth();
		void smooth(Mesh& dst) const;

		void generateTorusCylinder(float height, float diameter,
								   float thicknessRatio, int thetaFaces);
		void generateCubicTorus(float outerSize, float innerSize, float width);
		void generateCustomCubicTorus(float x1, float y1, float x2, float y2);

		void splitFace(const vertex* vertices, int x, int y);
		void splitAllFaces(int x);
		void splitAllFacesDownToMaxSide(float maxSide);

	private:
		void add(const vertex& v);
		void addNoDup(const vertex& v, float epsilon);
	};
}

#endif // MESH_HH
