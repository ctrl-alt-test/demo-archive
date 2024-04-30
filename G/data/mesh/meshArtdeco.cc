#ifndef MESH_EXPOSE

#include "array.hh"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "spline.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "sys/msys_libc.h"

namespace Meshes
{

static void generatePillar(Mesh::MeshStruct & dest,
			   Mesh::MeshStruct & temp,
			   float pillarWidth, float pillarHeight, float pillarLength,
			   float pillarBaseDepth, float pillarBaseHeight, float pillarTopHeight,
			   float y, float xBase, float zBase, float xOffset, float zOffset)
{
  const Mesh::PaveFaces faces = (Mesh::PaveFaces)(Mesh::pave_left * (xOffset <= 0) |
						    Mesh::pave_right * (xOffset >= 0) |
						    Mesh::pave_back * (zOffset <= 0) |
						    Mesh::pave_front * (zOffset >= 0));

  temp.vertices.empty();
  Mesh::Pave(pillarWidth, pillarHeight, pillarLength)
    .generateMesh(temp, faces);
  temp.translate(xBase + 0.5f * xOffset * pillarWidth,
		 y + 0.5f * pillarHeight + pillarBaseHeight,
		 zBase + 0.5f * zOffset * pillarLength);
  dest.add(temp);

  const float pillarBaseWidth = pillarWidth + (1.f + msys_fabsf(zOffset)) * pillarBaseDepth;
  const float pillarBaseLength = pillarLength + (1.f + msys_fabsf(xOffset)) * pillarBaseDepth;

  temp.vertices.empty();
  Mesh::Pave(pillarBaseWidth, pillarBaseHeight, pillarBaseLength)
    .generateMesh(temp, (Mesh::PaveFaces)(faces | Mesh::pave_top));
  temp.translate(xBase + 0.5f * xOffset * pillarBaseWidth,
		 y + 0.5f * pillarBaseHeight,
		 zBase + 0.5f * zOffset * pillarBaseLength);
  dest.add(temp);

  temp.vertices.empty();
  Mesh::Pave(pillarBaseWidth, pillarTopHeight, pillarBaseLength)
    .generateMesh(temp, (Mesh::PaveFaces)(faces | Mesh::pave_bottom));
  temp.translate(xBase + 0.5f * xOffset * pillarBaseWidth,
		 y + 0.5f * pillarTopHeight + pillarBaseHeight + pillarHeight,
		 zBase + 0.5f * zOffset * pillarBaseLength);
  dest.add(temp);
}

void buildArtdeco()
{
  Mesh::clearPool();

  const float buildingWidth = _TV(30.f);
  const float buildingLength = _TV(25.f);
  const float buildingHeight = _TV(50.f);

  const int numberOfFloors = _TV(10);
  const int numberOfFloorCeilLayers = _TV(3);
  const int numberOfPillarPerFloor = _TV(6);

  const float floorHeight = buildingHeight / numberOfFloors;

  const float floorCeilDepth = _TV(0.3f);
  const float floorCeilDepthDelta = _TV(0.07f);
  const float floorCeilLayerHeight = _TV(0.3f);

  const float pillarWidth = _TV(0.8f);
  const float pillarDepth = _TV(0.1f);

  const float pillarBaseHeight = _TV(0.3f);
  const float pillarTopHeight = _TV(0.15f);
  const float pillarBaseDepth = _TV(0.05f);

  const float pillarHeight = floorHeight - pillarBaseHeight - pillarTopHeight - floorCeilLayerHeight * numberOfFloorCeilLayers;

  Mesh::MeshStruct & building = Mesh::getTempMesh();
  Mesh::MeshStruct & floors = Mesh::getTempMesh();
  Mesh::MeshStruct & pillars = Mesh::getTempMesh();

  const Mesh::PaveFaces faces = (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom);

  Mesh::Pave(buildingWidth, buildingHeight, buildingLength).generateMesh(building, faces);
  building.translate(0, 0.5f * buildingHeight, 0);
  building.computeBaryCoord();

  Mesh::MeshStruct & element = Mesh::getTempMesh();

  const float xMin = -0.5f * buildingWidth;
  const float xMax = 0.5f * buildingWidth;
  const float zMin = -0.5f * buildingLength;
  const float zMax = 0.5f * buildingLength;

  for (int floor = 0; floor < numberOfFloors; ++floor)
  {
    for (int layer = 0; layer < numberOfFloorCeilLayers; ++ layer)
    {
	const float depth = floorCeilDepth - layer * floorCeilDepthDelta;
	element.vertices.empty();
	Mesh::Pave(buildingWidth + 2.f * depth,
		   floorCeilLayerHeight,
		   buildingLength + 2.f * depth)
	  .generateMesh(element);
	element.translate(0, (floor + 1) * floorHeight - (0.5f + layer) * floorCeilLayerHeight, 0);
	element.computeBaryCoord();

	// On augmente la résolution et on jette les faces qui sont
	// entièrement à l'intérieur du bâtiement
	{
	  element.splitAllFacesDownToMaxSide(1.f);
	  int i = 0;
	  while (i < element.vertices.size)
	  {
	    bool keep = false;
	    for (int j = 0; j < 4; ++j)
	      if (element.vertices[i + j].p.x <= xMin ||
		  element.vertices[i + j].p.x >= xMax ||
		  element.vertices[i + j].p.z <= zMin ||
		  element.vertices[i + j].p.z >= zMax)
	      {
		keep = true;
		break;
	      }
	    if (keep)
	    {
	      i += 4;
	    }
	    else
	    {
	      element.removeFace(i);
	    }
	  }
	}
	floors.add(element);
    }

    const float y = floor * floorHeight;

    for (int pillar = 0; pillar < numberOfPillarPerFloor; ++pillar)
    {
	const float xPillar = ((pillar + 0.5f) / numberOfPillarPerFloor - 0.5f) * buildingWidth;
	const float zPillar = ((pillar + 0.5f) / numberOfPillarPerFloor - 0.5f) * buildingLength;

	// Devant
	generatePillar(pillars, element,
		       pillarWidth, pillarHeight, pillarDepth,
		       pillarBaseDepth, pillarBaseHeight, pillarTopHeight,
		       y, xPillar, zMax, 0, 1.f);

// 	// Derrière
// 	generatePillar(pillars, element,
// 		       pillarWidth, pillarHeight, pillarDepth,
// 		       pillarBaseDepth, pillarBaseHeight, pillarTopHeight,
// 		       y, xPillar, zMin, 0, -1.f);

// 	// Gauche
// 	generatePillar(pillars, element,
// 		       pillarDepth, pillarHeight, pillarWidth,
// 		       pillarBaseDepth, pillarBaseHeight, pillarTopHeight,
// 		       y, xMin, zPillar, -1.f, 0);

	// Droite
	generatePillar(pillars, element,
		       pillarDepth, pillarHeight, pillarWidth,
		       pillarBaseDepth, pillarBaseHeight, pillarTopHeight,
		       y, xMax, zPillar, 1.f, 0);
    }
  }

  pillars.computeBaryCoord();
  pillars.splitAllFacesDownToMaxSide(0.5f);

  building.setId(0.f);
  floors.setId(1.f);
  pillars.setId(2.f);
  building.add(floors);
  building.add(pillars);

  SETUP_VBO(artDecoBuilding, building);
}

}

#elif MESH_EXPOSE == MESH_BUILDER_HEADER

void buildArtdeco();

#elif MESH_EXPOSE == MESH_BUILDER_INFO

{buildArtdeco DBGARG(__FILE__)},

#endif
