#include "Viaduct.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/PerlinNoise.hh"

#define ENABLE_VIADUCT_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Viaduct);
ObjectId Viaduct::objectId;
Viaduct* Viaduct::instance = NULL;
void Viaduct::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_VIADUCT_MESH

const float totalWidth = 9.f;
const float totalHeight = 80.f;
const float totalLength = 250.f;

const float firstDeckThickness = 3.f;
const float pillarTopThickness = 4.f;
const float pillarBottomThickness = 6.f;
const float pillarDistance = 20.f;

const float secondDeckHeight = 12.f;
const float secondDeckWidth = 7.f;
const float secondDeckThickness = 2.f;
const float secondPillarThickness = 1.8f;
const float secondPillarDistance = pillarDistance / 3.f;

#endif // ENABLE_VIADUCT_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Viaduct::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Viaduct(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_VIADUCT_MESH
	struct local
	{
		static void buildDeck(Mesh& dest, Mesh& part, Mesh& damagedPart,
			float width, float height, float length,
			float deckThickness,
			float pillarTopThickness, float pillarBottomThickness, float pillarDistance,
			float archRatio,
			int archSteps)
		{
			const float archHeight = archRatio * (pillarDistance - pillarTopThickness);
			const float pillarHeight = height - deckThickness - archHeight;

			// Step 0: Start with a cube.
			part.Clear();
			cube(part)
				//*
				// Step 0.1: Start with a flattened cube.
				.Translate(-0.5f, 0.f, -0.5f)
				.Scale(width, 0.f, pillarBottomThickness)
				// */
				;


			// Step 1: Stretch cube into a pillar.
			/*
			part.transformFace(12, matrix4::translation(0.f, pillarHeight, 0.f).
				scale(1.f, 1.f, pillarTopThickness/pillarBottomThickness));
			part.Extrude(12); 


			// Step 7 (1 bis): Split pillar in levels for damage.
			/*/
			float baseThickness = pillarBottomThickness;
			int verticalFaces = msys_ifloorf(height / 2.f);
			for (int i = 1; i <= verticalFaces; ++i)
			{
				float height = pillarHeight / verticalFaces;
				float thickness = mix(pillarBottomThickness, pillarTopThickness, i / float(verticalFaces));
				part.transformFace(12, matrix4::translation(0.f, height, 0.f).
					scale(1.f, 1.f, thickness/baseThickness));
				part.Extrude(12); 
				baseThickness = thickness;
			}
			// */


			// Step 2: Make an arch at the top of the pillar.
			//*
			float currentThickness = pillarTopThickness;
			float currenty = 0.f;
			for (int i = 1; i <= archSteps; ++i)
			{
				const float y = msys_sinf(PI / 2.f * float(i) / float(archSteps));
				const float x = 1.f - msys_cosf(PI / 2.f * float(i) / float(archSteps));
				const float targetThickness = mix(pillarTopThickness, pillarDistance, x);
				part.Extrude(12); part.transformFace(12, matrix4::translation(0.f, (y - currenty) * archHeight, 0.f).scale(1.f, 1.f, targetThickness / currentThickness));
				currentThickness = targetThickness;
				currenty = y;
			}


			// Step 3: Add the deck on top.
			//*
			part.Extrude(12); part.transformFace(12, matrix4::translation(0.f, deckThickness, 0.f));
			// */
			part.sharpenEdges();


			// Step 4: Repeat!
			/*
			float f = length/2.f;
			/*/
			for (float f = 0; f <= length; f += pillarDistance)
			// */
			{
				float position = f - 0.5f * length;

				damagedPart.Clear();

				// Step 5: Move it down.
				/*
				damagedPart.add(part);
				/*/
				damagedPart.add(part, matrix4::translation(0.f, -height, 0.f));
				//*/

				// Step 8: Add damage.
				//*
				float noiseIndex = f / length;
				float noiseFrequency = 4.f;
				float noise = Noise::PerlinNoise::value(noiseIndex * noiseFrequency, noiseFrequency);
				float damageIntensity = clamp(mix(-0.1f, 1.f, noise*noise), 0.f, 1.f);
				float signedDamage = noise * interpolate(0.5f, 1.f, damageIntensity);

				for (int i = 0; i < damagedPart.vertices.size; ++i)
				{
					vector3f& p = damagedPart.vertices[i].p;

					float damage = 1.f - interpolate(0.3f, 0.35f, 1.f - damageIntensity - p.y / totalHeight);
					damage = 1.f - damage*damage*damage*damage;
					p.x *= damage;
					p.z *= damage;

					// This function happened to be the only one to use 3D Perlin.
					// By using a 2D Perlin instead, we save ~240 bytes.
#if 1
					float noise2 = Noise::PerlinNoise::value(vec2(p.y, p.z+position), length) * 0.5f + 0.5f;
#else
					float noise2 = Noise::PerlinNoise::value(vec3(p.x, p.y, p.z+position), length) * 0.5f + 0.5f;
#endif

					// Move damaged vertices down.
					p.y -= 4.f * Algebra::abs(damageIntensity) * noise2;

					// Slanted columns.
					p.x += 10.f * signedDamage * (1.f + p.y / totalHeight);
				}
				// */
				dest.add(damagedPart, matrix4::translation(0.f, 0.f, position));
			}
		}
	};

	mesh::Mesh& part = pool.GetTempMesh();
	mesh::Mesh& damagedPart = pool.GetTempMesh();
	local::buildDeck(dest, part, damagedPart,
		totalWidth, totalHeight - secondDeckHeight, totalLength, firstDeckThickness,
		pillarTopThickness, pillarBottomThickness, pillarDistance, 0.3f, 10);

	// Step 6: Add a second deck.
	//*
	dest.Translate(0.f, -secondDeckHeight, 0.f);
	local::buildDeck(dest, part, damagedPart,
		secondDeckWidth, secondDeckHeight, totalLength, secondDeckThickness,
		secondPillarThickness, secondPillarThickness, secondPillarDistance, 0.5f, 5);
	// */

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
#endif // ENABLE_VIADUCT_MESH
}
