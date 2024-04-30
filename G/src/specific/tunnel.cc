
#include "array.hxx"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "node.hxx"
#include "renderable.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

namespace Tunnel
{
  void generateMeshes()
  {
    Mesh::clearPool();

    const float sliceLength = _TV(30.f);
    const float insideHeight = _TV(5.f);
    const float insideWidth = _TV(16.f);
    const float openRoofWidth = _TV(10.f);
    const float wallThickness = _TV(3.f);
    const float halfWallHeight = _TV(2.f);
    const float separationHeight = _TV(0.5f);
    const float pillarThickness = _TV(1.f);

    const int numberOfOpenWallPillars = _TV(4);

    Mesh::MeshStruct & slice1 = Mesh::getTempMesh();
    Mesh::MeshStruct & slice2 = Mesh::getTempMesh();
    Mesh::MeshStruct & slice3 = Mesh::getTempMesh();

    Mesh::MeshStruct & floor = Mesh::getTempMesh();

    Mesh::Pave groundBlock(insideWidth, wallThickness, sliceLength);
    groundBlock.generateMesh(floor, Mesh::pave_top);
    floor.translate(0, -0.5f * wallThickness, 0);
    slice1.add(floor);
    slice2.add(floor);
    slice3.add(floor);

    Mesh::MeshStruct & roof = Mesh::getTempMesh();
    groundBlock.generateMesh(roof, (Mesh::PaveFaces)(Mesh::pave_top | Mesh::pave_bottom | Mesh::pave_left));
    roof.translate(0, insideHeight + 0.5f * wallThickness, 0);
    slice1.add(roof);

    Mesh::MeshStruct & roof3 = Mesh::getTempMesh();
    groundBlock.generateMesh(roof3, (Mesh::PaveFaces)(Mesh::pave_top | Mesh::pave_bottom | Mesh::pave_left | Mesh::pave_front));
    roof3.translate(0, insideHeight + 0.5f * wallThickness, 0);
    slice3.add(roof3);

    Mesh::MeshStruct & openRoof = Mesh::getTempMesh();
    Mesh::Pave(openRoofWidth, wallThickness, sliceLength)
      .generateMesh(openRoof, (Mesh::PaveFaces)(Mesh::pave_top | Mesh::pave_bottom | Mesh::pave_left));
    openRoof.translate(0.5f * (insideWidth - openRoofWidth), insideHeight + 0.5f * wallThickness, 0);
    slice2.add(openRoof);

    Mesh::MeshStruct & rWall = Mesh::getTempMesh();
    Mesh::Pave wallBlock(wallThickness, insideHeight, sliceLength);
    wallBlock.generateMesh(rWall, Mesh::pave_left);
    rWall.translate(0.5f * (insideWidth + wallThickness), 0.5f * insideHeight, 0);
    slice1.add(rWall);
    slice2.add(rWall);
    slice3.add(rWall);

    Mesh::MeshStruct & lWall = Mesh::getTempMesh(); // 3 faces pour l'ombre
    wallBlock.generateMesh(lWall, (Mesh::PaveFaces)(Mesh::pave_top | Mesh::pave_left | Mesh::pave_right));
    lWall.translate(-0.5f * (insideWidth + wallThickness), 0.5f * insideHeight, 0);
    slice1.add(lWall);

    Mesh::MeshStruct & lWall3 = Mesh::getTempMesh();
    Mesh::Pave(wallThickness, insideHeight + wallThickness, sliceLength)
      .generateMesh(lWall3, (Mesh::PaveFaces)(Mesh::pave_left | Mesh::pave_right | Mesh::pave_front));
    lWall3.translate(-0.5f * (insideWidth + wallThickness), 0.5f * (insideHeight + wallThickness), 0);
    slice3.add(lWall3);

    Mesh::MeshStruct & halfWall = Mesh::getTempMesh();
    Mesh::Pave(wallThickness, halfWallHeight, sliceLength)
      .generateMesh(halfWall, (Mesh::PaveFaces)(Mesh::pave_top | Mesh::pave_left | Mesh::pave_right));
    halfWall.translate(-0.5f *  (insideWidth + wallThickness), 0.5f * halfWallHeight, 0);
    slice2.add(halfWall);

    Mesh::MeshStruct & bottomSeparation = Mesh::getTempMesh();
    Mesh::Pave separationBlock(wallThickness, separationHeight, sliceLength);
    separationBlock.generateMesh(bottomSeparation, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
    bottomSeparation.translate(0, 0.5f * separationHeight, 0);
    slice1.add(bottomSeparation);

    Mesh::MeshStruct & topSeparation = Mesh::getTempMesh();
    separationBlock.generateMesh(topSeparation, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_top));
    topSeparation.translate(0, insideHeight - 0.5f * separationHeight, 0);
    slice1.add(topSeparation);

    Mesh::MeshStruct & pillar = Mesh::getTempMesh();
    const float pillarHeight = insideHeight - 2.f * separationHeight;
    Mesh::Revolution(Mesh::pipeHFunc, Mesh::pipeRFunc)
      .generateMesh(pillar, pillarHeight, _TV(0.5f) * pillarThickness, _TV(1), _TV(-9));
    pillar.translate(0, separationHeight, -0.5f * sliceLength);
    slice1.add(pillar);


    Mesh::MeshStruct & openWallPillar = Mesh::getTempMesh();
    const float openWallPillarHeight = insideHeight - halfWallHeight;
    Mesh::Pave(pillarThickness, openWallPillarHeight, pillarThickness)
      .generateMesh(openWallPillar, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
    openWallPillar.translate(-0.5f * (insideWidth + wallThickness),
			     halfWallHeight + 0.5f * openWallPillarHeight,
			     -0.5f * (1.f - 1.f / numberOfOpenWallPillars) * sliceLength);

    Mesh::MeshStruct & linteau = Mesh::getTempMesh();
    const float linteauWidth = insideWidth - openRoofWidth + 0.5f * (wallThickness + pillarThickness);
    Mesh::Pave(linteauWidth, pillarThickness, pillarThickness)
      .generateMesh(linteau, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_right));
    linteau.translate(0.5f * (insideWidth - linteauWidth) - openRoofWidth,
		      insideHeight + 0.5f * pillarThickness,
		      -0.5f * (1.f - 1.f / numberOfOpenWallPillars) * sliceLength);
    for (int i = 0; i < numberOfOpenWallPillars; ++i)
    {
      slice2.add(openWallPillar);
      slice2.add(linteau);
      openWallPillar.translate(0, 0, sliceLength / numberOfOpenWallPillars);
      linteau.translate(0, 0, sliceLength / numberOfOpenWallPillars);
    }


    // Les beat pillars sont des pilliers qui représentant les coups
    // de caisse claire à la fin de la séquence tunnel
    const int numberOfBeatPillars = _TV(5);

    Mesh::MeshStruct & beatPillar = Mesh::getTempMesh();
    const float beatPillarHeight = insideHeight;

    Mesh::Pave beatPillarBlock(pillarThickness, beatPillarHeight, pillarThickness);
    beatPillarBlock.generateMesh(beatPillar);
    beatPillar.translate(insideWidth, 0, 0);
    beatPillarBlock.generateMesh(beatPillar);
    beatPillar.translate(-0.5f * insideWidth,
			 0.5f * beatPillarHeight,
			 -0.5f * (1.f - 1.f / numberOfBeatPillars) * sliceLength);
    for (int i = 0; i < numberOfBeatPillars; ++i)
    {
      slice3.add(beatPillar);
      beatPillar.translate(0, 0, sliceLength / numberOfBeatPillars);
    }

    slice1.setColor(Material::tunnelColor);
    slice2.setColor(Material::tunnelColor);
    slice3.setColor(Material::tunnelColor);
    slice1.computeBaryCoord();
    slice2.computeBaryCoord();
    slice3.computeBaryCoord();
    SETUP_VBO(tunnel1Slice, slice1);
    SETUP_VBO(tunnel2Slice, slice2);
    SETUP_VBO(tunnel3Slice, slice3);


    const int numberOfLightBulbs = _TV(7);
    const float lightBulbLength = _TV(1.f);
    const float lightBulbThickness = _TV(0.5f);

    Mesh::MeshStruct & lightBulbs1 = Mesh::getTempMesh();
    Mesh::MeshStruct & lightBulbs2 = Mesh::getTempMesh();

    Mesh::MeshStruct & lLightBulb = Mesh::getTempMesh();
    Mesh::MeshStruct & rLightBulb = Mesh::getTempMesh();
    Mesh::Pave lightBlock(lightBulbThickness, lightBulbThickness, lightBulbLength);
    lightBlock.generateMesh(lLightBulb);
    lightBlock.generateMesh(rLightBulb);

    lLightBulb.setColor(Material::tunnelColor);
    rLightBulb.setColor(Material::tunnelColor);

    for (int i = _TV(0); i < _TV(4); ++i)
    {
      const vector3f & color = Material::cityLightColor;
      lLightBulb.vertices[i].r = color.x;
      lLightBulb.vertices[i].g = color.y;
      lLightBulb.vertices[i].b = color.z;

      rLightBulb.vertices[i].r = color.x;
      rLightBulb.vertices[i].g = color.y;
      rLightBulb.vertices[i].b = color.z;
    }


    lLightBulb.rotate(DEG_TO_RAD * _TV(45.f), 0, 0, 1.f);
    rLightBulb.rotate(DEG_TO_RAD * _TV(45.f), 0, 0, 1.f);
    lLightBulb.translate(-0.5f * insideWidth - _TV(0.125f) * lightBulbThickness,
			 insideHeight - _TV(0.125f) * lightBulbThickness,
			 -0.5f * (1.f - 1.f / numberOfLightBulbs) * sliceLength);
    rLightBulb.translate(0.5f * insideWidth - _TV(0.125f) * lightBulbThickness,
			 insideHeight - _TV(0.125f) * lightBulbThickness,
			 -0.5f * (1.f - 1.f / numberOfLightBulbs) * sliceLength);

    for (int i = 0; i < numberOfLightBulbs; ++i)
    {
      lightBulbs1.add(lLightBulb);
      lightBulbs1.add(rLightBulb);
      lightBulbs2.add(rLightBulb);
      lLightBulb.translate(0, 0, sliceLength / numberOfLightBulbs);
      rLightBulb.translate(0, 0, sliceLength / numberOfLightBulbs);
    }

    lightBulbs1.computeBaryCoord();
    lightBulbs2.computeBaryCoord();
    SETUP_VBO(tunnel1SliceLights, lightBulbs1);
    SETUP_VBO(tunnel2SliceLights, lightBulbs2);
  }

  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);

    Renderable tunnel1(Material::retro, VBO_(tunnel1Slice));
    Renderable tunnel2(Material::retro, VBO_(tunnel2Slice));
    Renderable tunnel3(Material::retro, VBO_(tunnel3Slice));
    Renderable lights1(Material::retro, VBO_(tunnel1SliceLights));
    Renderable lights2(Material::retro, VBO_(tunnel2SliceLights));

    // Dans la musique, le passage avec le tunnel a 32 battements
    // (plus le premier et le dernier), et le changement de ton
    // intervient à mi-chemin. Il nous faut donc deux séries de 16
    // tronçons.
    const int numberOfSlices = _TV(16);
    const float sliceLength = _TV(30.f);
    for (int i = 0; i < 2 * numberOfSlices + 1; ++i)
    {
      glLoadIdentity();
      glTranslatef(0, 0, -i * sliceLength);
      if (i < numberOfSlices)
      {
	root->attachRenderableNode(tunnel1);
	root->attachRenderableNode(lights1);
      }
      else if (i < 2 * numberOfSlices - 1)
      {
	root->attachRenderableNode(tunnel2);
	root->attachRenderableNode(lights2);
      }
      else
      {
	root->attachRenderableNode(tunnel3);
      }
    }

    // On attache un bout de ville pour le décor
    glLoadIdentity();
    glTranslatef(_TV(-450.f), 0.f, _TV(-500.f));
    glRotatef(90.f, 0.f, 1.f, 0.f);
    Renderable cityPatch(Material::retro, (VBO::id)(VBO_(city) + _TV(2)));
    Node * n = Node::New(startDate, _TV(253200));
    n->attachRenderable(cityPatch);
    n->attachToNode(root);

    return root;
  }
}
