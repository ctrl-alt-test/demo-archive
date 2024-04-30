#include "ThreejsExporter.hh"

#include "engine/mesh/Mesh.hh"
#include <cstring>
#include <cassert>
#include <cstdio>

#if DEBUG

using namespace Tool;

void ThreejsExporter::SaveToJSON(const mesh::Mesh& mesh, char* fileName)
{
	// https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3
	// TODO: export material
	// TODO: export normals
	FILE* fp = fopen(fileName, "w");
	assert(fp != NULL);

	char header[] =
		"{\n"
		"\"metadata\": { \"formatVersion\" : 3 },\n"
		"\n"
		"\"materials\": [{\n"
		"  \"colorAmbient\" : [0.8, 0.8, 0.8],\n"
		"  \"colorDiffuse\" : [0.03, 0.3, 0.06],\n"
		"  \"colorSpecular\" : [0.5, 0.5, 0.5],\n"
		"  \"shading\" : \"Lambert\",\n"
		"  \"specularCoef\" : 10\n"
		"  }],\n"
		"\n";
	fprintf(fp, "%s", header);

	fprintf(fp, "\"vertices\": [\n");
	for (int i = 0; i < mesh.vertices.size; i++)
	{
		const Algebra::vector3f& p = mesh.vertices[i].p;
		fprintf(fp, "%f,%f,%f", p.x, p.y, p.z);
		if (i + 1 < mesh.vertices.size)
		{
			fprintf(fp, ",\n");
		}
	}
	fprintf(fp, "],\n");

	fprintf(fp, "\"normals\": [\n");
	for (int i = 0; i < mesh.vertices.size; i++)
	{
		const Algebra::vector3f& n = mesh.vertices[i].n;
		fprintf(fp, "%f,%f,%f", n.x, n.y, n.z);
		if (i + 1 < mesh.vertices.size)
		{
			fprintf(fp, ",\n");
		}
	}
	fprintf(fp, "],\n");

	fprintf(fp, "\"faces\": [\n");
	for (int i = 0; i < mesh.quads.size; i += 4)
	{
		int kind = 1 | 31; // quad, normals
		fprintf(fp, "%d, %d,%d,%d,%d, %d,%d,%d,%d", kind,
				mesh.quads[i], mesh.quads[i+1], mesh.quads[i+2], mesh.quads[i+3],  // vertex index
				mesh.quads[i], mesh.quads[i+1], mesh.quads[i+2], mesh.quads[i+3]); // normal index
		if (i + 4 < mesh.quads.size)
		{
			fprintf(fp, ",\n");
		}
	}
	fprintf(fp, "]}\n");

	fclose(fp);
}

#endif
