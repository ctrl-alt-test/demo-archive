#ifndef THREEJS_EXPORTER_HH
#define THREEJS_EXPORTER_HH

namespace mesh
{
	struct Mesh;
};

namespace Tool
{
	class ThreejsExporter
	{
	public:
		static void SaveToJSON(const mesh::Mesh& mesh, char* fileName);
	};
}

#endif // THREEJS_EXPORTER_HH
