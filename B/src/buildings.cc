//
// Buildings divers
//

#include "buildings.hh"
#include "timing.hh"

namespace Building
{
  void pacmanHouse(RenderList & renderList, date birthDate, date deathDate)
  {
    glPushMatrix();
    glTranslatef(0, 0.5f, 0);
    Renderable house(deathDate, Shader::bump, birthDate);
    house.setTextures(Texture::pacmanWall, Texture::pacmanWallBump, Texture::pacmanWallSpecular);
    renderList.add(house);
    glPopMatrix();
  }

  //
  // renderLists est un tableau de RenderList
  // Les éléments de l'immeuble iront dans l'une des listes, en
  // fonction de la texture.
  // Il faut autant de listes que de textures (albedo) différentes
  //
  void stackUp(Array <RenderList *> & renderLists,
	       float size, int id, bool isRoof)
  {
    // id permet de modifier la couleur du batiment
    // si id < 0, on ne met pas de fenetre

    // Types de VBO :
    //  0 - mur sans fenêtre
    //  1 - fenêtre 1
    //  2 - fenêtre 2
    //  3 - fenêtre 3
    //  4 - fenêtre 4
    //  1 - toit 1
    //  2 - toit 2
    //  3 - toit 3
    //  4 - toit 4

    unsigned int kind = 0;
    if (isRoof)
    {
      kind = 5 + (id < 0 ? 0 : id % 4);
    }
    else
    {
      kind = (id < 0 ? 0 : 1 + id % 4);
    }

    glPushMatrix();
    glScalef(size, size, size);
    glTranslatef(0, 0.5f, 0);

    renderLists[kind]->add(Renderable(-1));

    glPopMatrix();
    glTranslatef(0, size, 0);
  }

  void buildTower(Array <RenderList *> & renderLists,
		  const towerPart * description, unsigned int parts)
  {
    for (unsigned int j = 0; j < parts; ++j)
    {
      const towerPart & part = description[j];
      glPushMatrix();
      glTranslatef(part.x, 0, part.y);
      for (unsigned int i = 0; i * part.side < part.height; ++i)
      {
	Building::stackUp(renderLists, part.side, part.id, false);
      }
      glTranslatef(0, -part.side, 0);
      Building::stackUp(renderLists, part.side, part.id, true);
      glPopMatrix();
    }
  }

  /*
  //
  // En référence à la Landmark Tower, à Yokohama
  //
  void addLandmarkTowerToList(Array <RenderList *> & renderLists, int id)
  {
    glPushMatrix();
    for (unsigned int i = 0; i < 3; ++i)
    {
      stackUp(renderList, 40.f, id);
    }
    for (unsigned int i = 0; i < 2; ++i)
    {
      stackUp(renderList, 35.f, id);
    }
    for (unsigned int i = 0; i < 3; ++i)
    {
      stackUp(renderList, 25.f, id);
    }
    glPopMatrix();
  }
  */

  //
  // Grosse tour vaguement ronde
  //
  void addRoundTowerToList(Array <RenderList *> & renderLists, float height, int id)
  {
    glPushMatrix();
    unsigned int i = 0;
    while (i * 20.f < height)
      ++i;
    glTranslatef(0, 5.f + 20.f * (i - 1), 0);
    Building::stackUp(renderLists, 20.f, id, false);
    glTranslatef(0, -20.f, 0);
    Building::stackUp(renderLists, 20.f, id, true);
    glPopMatrix();

    const towerPart parts[8] =
      {
	{height,   20.f,     0,  20.f, id},
	{height,  -20.f,     0,  20.f, id},
	{height,      0,  20.f,  20.f, id},
	{height,      0, -20.f,  20.f, id},

	{height,  16.5f,  16.5f, 13.f, -id},
	{height, -16.5f,  16.5f, 13.f, -id},
	{height,  16.5f, -16.5f, 13.f, -id},
	{height, -16.5f, -16.5f, 13.f, -id}
      };
    buildTower(renderLists, parts, 8);
  }

  //
  // Tour plutôt carrée inspirée de celle à côté de la Landmark Tower
  //
  void addAngularTowerToList(Array <RenderList *> & renderLists, float height, int id)
  {
    glPushMatrix();
    unsigned int i = 0;
    while (i * 20.f < height)
      ++i;
    glTranslatef(0, 20.f * (i - 1), 0);
    Building::stackUp(renderLists, 20.f, id, false);
    glPopMatrix();

    #define DIAG_POS 50.f/3.f
    #define DIAG_SIZE 40.f/3.f

    #define BORD_POS1 40.f/3.f
    #define BORD_POS2 80.f/3.f
    #define BORD_SIZE 20.f/3.f

    const towerPart parts[16] =
      {
	{height,  20.f,     0,  20.f, id},
	{height, -20.f,     0,  20.f, id},
	{height,     0,  20.f,  20.f, id},
	{height,     0, -20.f,  20.f, id},

	{0.8f * height,   DIAG_POS,   DIAG_POS, DIAG_SIZE, id},
	{0.8f * height,  -DIAG_POS,   DIAG_POS, DIAG_SIZE, id},
	{0.8f * height,   DIAG_POS,  -DIAG_POS, DIAG_SIZE, id},
	{0.8f * height,  -DIAG_POS,  -DIAG_POS, DIAG_SIZE, id},

	{0.7f * height,  BORD_POS1,  BORD_POS2, BORD_SIZE, id},
	{0.7f * height,  BORD_POS1, -BORD_POS2, BORD_SIZE, id},
	{0.7f * height, -BORD_POS1,  BORD_POS2, BORD_SIZE, id},
	{0.7f * height, -BORD_POS1, -BORD_POS2, BORD_SIZE, id},

	{0.7f * height,  BORD_POS2,  BORD_POS1, BORD_SIZE, id},
	{0.7f * height,  BORD_POS2, -BORD_POS1, BORD_SIZE, id},
	{0.7f * height, -BORD_POS2,  BORD_POS1, BORD_SIZE, id},
	{0.7f * height, -BORD_POS2, -BORD_POS1, BORD_SIZE, id}
      };
    buildTower(renderLists, parts, 16);
  }
}
