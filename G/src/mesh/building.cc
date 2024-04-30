//
// Bâtiment
//

#include "building.hh"

#include "cube.hh"
#include "pave.hh"
#include "randomness.hh"

#include "sys/msys_libc.h"

namespace Mesh
{
  //
  // Observations :
  //
  // Un taux d'asymmétrie entre 0 et 0,6 semble bien. Au delà ça fait trop aléatoire.
  // Un facteur de réduction entre 0,3 et 1 semble bien. En dessous c'est trop fin.
  // Un taux d'asymmétrie élevée marche mieux avec un facteur de
  // réduction faible, et inversement.
  // Les petits bâtiments rendent mieux avec presque pas de réduction.
  // Les grands bâtiments rendent aussi bien avec une petite qu'une grande réduction.
  // Les petits bâtiments ont besoin d'une surface plus petite que les grands.
  // Un espacement de 20% rend bien. 40% c'est trop.
  //

  //
  // Fonction pour générer un bâtiment
  //
  // vertices: tableau de sommets, non vide, de taille suffisante;
  //           les sommets crées sont ajoutés a la fin
  //
  // progression: pour animer la creation
  // 0: debut
  // 1: fin
  //
  // noise: liberté prise par rapport aux arguments sur la forme
  // 0: les valeurs sont prises pour argent comptant
  // 1: random total
  //
  // rand: générateur de nombre aléatoire à utiliser
  //
  // averageHieght: hauteur souhaitée
  //
  // maxWidth, maxLength: surface rectangulaire disponible
  // /* base: contour de la surface disponible */
  //
  // style: forme du bâtiment (à définir)
  //        on pourrait avoir N paramètres, 2 devraient suffire pour commencer;
  //        par exemple symmétrique ou non, base rectangulaire ou quelconque,
  //        style classique ou moderne, etc.
  //
  // lod: niveau de détail (simple pavé, ..., forme complètement détaillée)
  //
  //
  // Note : ce prototype pourrait être suffisant si dans une future démo
  // on veut faire deux villes identiques mais de styles différents
  // (façon Giana Sisters), ou une ville dont le style évolue, ou une
  // ville qui se construit (on pourrait même animer des grues juste en
  // jouant sur la progression, et ajouter des étages toutes les
  // fractions de temps).
  //
  static
  void generateBuilding(Mesh::MeshStruct & mesh,
			float id,
			float progression,
			float noise,
			Rand & rand,
			float averageHeight,
			float maxWidth, float maxLength,
// 			const Array<vector2f> & base,
			const vector2f& style,
			Building::levelOfDetail lod)
  {
    const float height = rand.fgenOnAverage(averageHeight, averageHeight * noise);
    const float reduction = 0.3f + 0.7f * style.x;
    const float asymmetry = 0.6f * style.y;// * style.x;

    const float floorHeight = 3.f;
    float baseHeight = height * rand.fgen(0.1f, 0.2f);
    baseHeight = msys_max(floorHeight, baseHeight - msys_fmodf(baseHeight, floorHeight));
    const float topHeight =  height * rand.fgen(0.05f, 0.1f);
    const float trunkHeight = height - baseHeight - topHeight;

    const float baseWidth =  rand.fgen(maxWidth * (1.f - noise), maxWidth);
    const float baseLength = rand.fgen(maxLength * (1.f - noise), maxLength);
    const float trunkWidth = baseWidth * rand.fgenOnAverage(reduction, reduction * noise);
    const float trunkLength = baseLength * rand.fgenOnAverage(reduction, reduction * noise);
    const float topWidth =  rand.fgen(trunkWidth * reduction * (1.f - noise), trunkWidth);
    const float topLength = rand.fgen(trunkLength * reduction * (1.f - noise), trunkLength);

    const float baseX =  rand.fgenOnAverage(0.f, asymmetry * 0.5f * (maxWidth - baseWidth));
    const float baseY =  rand.fgenOnAverage(0.f, asymmetry * 0.5f * (maxLength - baseLength));
    const float trunkX = rand.fgenOnAverage(0.f, asymmetry * 0.5f * (baseWidth - trunkWidth));
    const float trunkY = rand.fgenOnAverage(0.f, asymmetry * 0.5f * (baseLength - trunkLength));
    const float topX =   rand.fgenOnAverage(0.f, asymmetry * 0.5f * (trunkWidth - topWidth));
    const float topY =   rand.fgenOnAverage(0.f, asymmetry * 0.5f * (trunkLength - topLength));

    Mesh::MeshStruct building(20 * Cube::numberOfVertices);

    switch (lod)
    {
    case Building::minDetail:
      {
	Pave(baseWidth, height, baseLength).generateMesh(building, (PaveFaces)(pave_all & ~pave_bottom));
	building.translate(baseX + trunkX, 0.5f * height, baseY + trunkY);
	break;
      }
    case Building::medDetail:
    case Building::maxDetail:
      {
	Pave(topWidth, topHeight, topLength).generateMesh(building, (PaveFaces)(pave_all & ~pave_bottom));
	building.translate(topX, 0.5f * (topHeight + trunkHeight), topY);

	Pave(trunkWidth, trunkHeight, trunkLength).generateMesh(building, (PaveFaces)(pave_all & ~pave_bottom));
	building.translate(trunkX, 0.5f * (trunkHeight + baseHeight), trunkY);

	Pave(baseWidth, baseHeight, baseLength).generateMesh(building, (PaveFaces)(pave_all & ~pave_bottom));
	building.translate(baseX, 0.5f * baseHeight, baseY);
	break;
      }
    }
    building.setId(id);

    mesh.add(building);
  }

  void Building::generateMesh(Mesh::MeshStruct & mesh,
			      float id,
			      float progression,
			      float noise,
			      Rand & rand,
			      Building::levelOfDetail lod)
  {
    generateBuilding(mesh,
		     id,
		     progression,
		     noise,
		     rand,
		     _averageHeight,
		     _maxWidth, _maxLength,
// 		     _base,
		     _style,
		     lod);
  }
}
