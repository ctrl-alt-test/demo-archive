
#include "shelf.hh"

#include "sys/msys.h"

#include "mesh.hh"
#include "node.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Shelf
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable shelf(Shader::parallax, VBO::shelf);
    root->attachRenderable(shelf.setTextures(Texture::wood_Pauvre, Texture::woodBump1));

    Renderable toysPart1(Shader::parallax, VBO::toysPart1);
    toysPart1.setTextures(Texture::wood_TendreSec, Texture::woodBump1);
    toysPart1.setShininess(_TV(8));
    root->attachRenderable(toysPart1);

    Renderable toysPart2(Shader::parallax, VBO::toysPart2);
    toysPart2.setTextures(Texture::wood_Chene, Texture::woodBump1);
    toysPart2.setShininess(_TV(50));
    root->attachRenderable(toysPart2);

    Renderable toysPart3(Shader::parallax, VBO::toysPart3);
    toysPart3.setTextures(Texture::wood_Noyer, Texture::woodBump1);
    toysPart3.setShininess(_TV(80));
    root->attachRenderable(toysPart3);

    glLoadIdentity();
    Node * secondNode = Node::New(worldStartDate, worldEndDate);
    Renderable sign(Shader::parallax, VBO::sign);
    secondNode->attachRenderable(sign.setTextures(Texture::trainSign));

    Renderable boatFlag(Shader::parallax, VBO::boatFlag);
    secondNode->attachRenderable(boatFlag.setTextures(Texture::flag));

    secondNode->attachToNode(root);

    return root;
  }

  // Une réplique en bois de la célèbre statue d'Incubation.
  static void generateStatue(Array<vertex> & roughPart,
			     Array<vertex> & veneerClearPart,
			     Array<vertex> & veneerDarkPart,
			     Array<vertex> & temp)
  {
    temp.empty();
    Mesh::Revolution rev(Mesh::wheelHFunc, Mesh::wheelRFunc);

    const float x = _TV(-50.f);
    const float y = _TV(45.f);
    const float z = _TV(5.f);

    const float scale = _TV(2.f);
    const float woodTextureScale = _TV(0.1f);

    // Socle
    rev.generateMesh(temp, _TV(0.5f), _TV(0.25f), _TV(7), _TV(-8));
    glLoadIdentity();
    glRotatef(180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::applyCurrentProjection(temp);
    Mesh::expandPave(temp, _TV(1.8f) * scale, _TV(0.8f), _TV(1.8f) * scale);
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(x, y, z);
    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, temp);
    temp.empty();

    // Gros cubes
    rev.generateMesh(temp, _TV(0.25f), _TV(0.125f), _TV(7), _TV(-8));
    glLoadIdentity();
    glRotatef(180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::applyCurrentProjection(temp);
    Mesh::expandPave(temp, scale, scale, scale);
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(x, y + _TV(1.6f) * scale, z);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);

    glTranslatef(0, _TV(4.35f) * scale, 0);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);
    temp.empty();

    // Petit cube
    rev.generateMesh(temp, _TV(0.25f), _TV(0.125f), _TV(7), _TV(-8));
    glLoadIdentity();
    glRotatef(180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::applyCurrentProjection(temp);
    Mesh::expandPave(temp, 0.6f * scale, 0.6f * scale, 0.6f * scale);
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(x, y + _TV(3.8f) * scale, z);
    glRotatef(45.f, 1.f, 0.f, 1.f);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);
  }

  // Tours de Hanoï
  static void generateHanoi(Array<vertex> & roughPart,
			    Array<vertex> & veneerClearPart,
			    Array<vertex> & veneerDarkPart,
			    Array<vertex> & temp)
  {
    temp.empty();
    Mesh::Revolution rev(Mesh::wheelHFunc, Mesh::wheelRFunc);

    const float woodTextureScale = _TV(0.05f);


    // Socle
    rev.generateMesh(temp, 0.5f, 0.25f, 7, -8);
    glLoadIdentity();
    glRotatef(180.f / 8.f, 0, 1.f, 0);
    Mesh::applyCurrentProjection(temp);
    Mesh::expandPave(temp, 10.f, 0.25f, 3.f);
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(10.f), _TV(110.f), _TV(13.f));
    glRotatef(_TV(3.f), 0, 1.f, 0);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);
    temp.empty();

    // Tours
    glTranslatef(0.f, 3.5f, 0.f);
    rev.generateMesh(temp, 0.4f, 0.2f, 5, 6);
    Mesh::expandPave(temp, 0, 3.f, 0);
    Mesh::addVerticesUsingCurrentProjection(roughPart, temp);
    glTranslatef(6.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(roughPart, temp);
    glTranslatef(-12.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(roughPart, temp);
    temp.empty();

    // Disques
    glTranslatef(0.f, -3.3f, 0.f);
    for (int i = 7; i >= 0; --i)
    {
      rev.generateMesh(temp, 0.4f, (i+2) * 0.3f, 7, 16);
      Mesh::expandPave(temp, 0, 0.1f, 0);
      Mesh::reprojectTextureXZPlane(temp, woodTextureScale);
      Mesh::translateTexture(temp, msys_frand(), msys_frand());
      glTranslatef(0, 0.6f, 0);
      if (i % 2 == 0)
	Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);
      else
	Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, temp);
      temp.empty();
    }
  }

  static void createWheels(Array<vertex> & wheels,
			   Array<vertex> & temp,
			   float r, float h,
			   float xSpace, float zSpace,
			   int numberOfWheels = 4)
  {
    Mesh::Revolution rev(Mesh::wheelHFunc, Mesh::wheelRFunc);
    rev.generateMesh(temp, r, h, _TV(6), _TV(12));

    for (int i = 0; i < numberOfWheels; ++i)
    {
      const float x = xSpace * float(i / 2);
      const float z = zSpace * (i % 2 > 0 ? 0.5f : -0.5f);

      glLoadIdentity();
      glTranslatef(x, 0, z);
      glRotatef(90.f, 1.f, 0, 0);
      Mesh::addVerticesUsingCurrentProjection(wheels, temp);
    }
    Mesh::addNoise(wheels, _TV(0.05f));
    Mesh::reprojectTextureZYPlane(wheels, 0.05f);
    temp.empty();
  }

  static void generateWoodCar(Array<vertex> & roughPart,
			      Array<vertex> & veneerClearPart,
			      Array<vertex> & veneerDarkPart,
			      Array<vertex> & body)
  {
    body.empty();
    Mesh::generatePave(body, _TV(15.f), _TV(5.f), _TV(6.f)); // pour avoir le cube dans un array
    Mesh::splitAllFaces(body, 3);

    // on abaisse l'avant de la voiture
    for (int i = 0; i < body.size; i++)
    {
      if (body[i].p.x > 0 && body[i].p.y > 0)
        body[i].p.y = _TV(0.f);
    }
    Mesh::addNoise(body, _TV(0.125f));

    // Ensemble des roues
    Array<vertex> & wheel = Mesh::getTemp();
    Array<vertex> & wheels = Mesh::getTemp();
    createWheels(wheels, wheel, _TV(1.5f), _TV(2.f), _TV(10.f), _TV(8.f));

    glLoadIdentity();
    glTranslatef(_TV(-15.f), _TV(46.f), _TV(4.f));
    glRotatef(_TV(-72.f), 0, 1.f, 0);
    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, wheels);

    glTranslatef(_TV(5.f), _TV(2.f), _TV(0.f));
    Mesh::addNoise(body, _TV(0.05f));
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, body);
  }

  static float sportCarData[] = {
    0, 20,   0,
    0, 18,  32,
    0, 2,   40,
    0, 3,   60,
    0, 30,  90,
    0, 65, 100,
    0, 90,  72,
    0, 98,  40,
    0, 100,  0,
  };

  static void generateRetroSportCar(Array<vertex> & roughPart,
				    Array<vertex> & veneerClearPart,
				    Array<vertex> & veneerDarkPart,
				    Array<vertex> & temp)
  {
    temp.empty();
    Array<vertex> & wheels = Mesh::getTemp();
    Mesh::Revolution body(NULL, NULL, sportCarData, ARRAY_LEN(sportCarData));
    Mesh::Revolution head(Mesh::sphereHFunc, Mesh::sphereRFunc);

    const float woodTextureScale = _TV(0.05f);

    createWheels(wheels, temp, _TV(1.f), _TV(2.f), _TV(8.f), _TV(7.f));

    body.generateMesh(temp, _TV(16.f), _TV(3.f), _TV(20), _TV(24));
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(-36.f), _TV(46.f), _TV(9.f));
    glRotatef(_TV(-53.f), 0, 1.f, 0);
    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, wheels);
    glTranslatef(_TV(12.f), _TV(.8f), _TV(0.f));
    glRotatef(_TV(85.f), 0, 0, 1.f);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);

    //
    // FIXME : aplatir le chassis
    //

    //
    // FIXME : tête et parebrise
    //

    temp.empty();

    head.generateMesh(temp, _TV(1.f), _TV(1.f), _TV(8), _TV(8));
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glTranslatef(_TV(0.f), _TV(5.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(roughPart, temp);
  }

  /*
  static void generateCanon(Array<vertex> & roughPart,
			    Array<vertex> & veneerClearPart,
			    Array<vertex> & veneerDarkPart,
			    Array<vertex> & temp)
  {
    temp.empty();
    Array<vertex> & wheels = Mesh::getTemp();
    Mesh::Revolution body(NULL, NULL, sportCarData, ARRAY_LEN(sportCarData));
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);

    const float woodTextureScale = _TV(0.05f);

    createWheels(wheels, temp, _TV(1.f), _TV(2.f), 0, _TV(6.f), 2);

    body.generateMesh(temp, _TV(10.f), _TV(2.f), _TV(10), _TV(10));
    Mesh::reprojectTextureXZPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(50.f), _TV(46.f), _TV(5.f));
    glRotatef(_TV(10.f), 0, 1.f, 0);
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, wheels);
    glTranslatef(_TV(-4.f), _TV(0.f), _TV(-4.f));
    glRotatef(_TV(60.f), 0, 0, _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, temp);

    //
    // FIXME : couronne, mèche, boulets, attelage, socle
    //
  }
  */

  static float bilboquetData[] = {
    0, 0, 80,
    0, 5, 70,
    0, 9, 30,
    0, 9, 30,
    0, 13, 38,
    0, 13, 38,
    0, 16, 30,
    0, 21, 30,
    0, 38, 42,
    0, 43, 42,
    0, 47, 30,
    0, 47, 30,
    0, 49, 40,
    0, 51, 40,
    0, 53, 30,
    0, 53, 30,
    0, 57, 42,
    0, 62, 42,
    0, 79, 30,
    0, 84, 30,
    0, 87, 38,
    0, 87, 38,
    0, 91, 30,
    0, 91, 30,
    0, 95, 40,
    0, 96, 40,
    0, 98, 50,
    0, 100, 50,
  };

  static void generateBilboquet(Array<vertex> & roughPart,
				Array<vertex> & veneerClearPart,
				Array<vertex> & veneerDarkPart,
				Array<vertex> & temp)
  {
    const float woodTextureScale = _TV(0.1f);

    temp.empty();
    Mesh::Revolution body(NULL, NULL, bilboquetData, ARRAY_LEN(bilboquetData));
    body.generateMesh(temp, _TV(14.f), _TV(3.f), _TV(40), _TV(12));
    Mesh::reprojectTextureZYPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(25.f), _TV(110.f), _TV(0.f));
    glRotatef(_TV(65.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);


    temp.empty();
    Mesh::Revolution ball(Mesh::sphereHFunc, Mesh::sphereRFunc);
    ball.generateMesh(temp, _TV(4.f), _TV(4.f), _TV(12), _TV(12));
    Mesh::reprojectTextureZYPlane(temp, woodTextureScale);

    glTranslatef(_TV(0.f), _TV(17.7f), _TV(0.f));
    glRotatef(_TV(40.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(veneerClearPart, temp);

    //
    // FIXME: corde
    //
  }

  static float toupieData[] = {
    0, 0, 0,
    0, 5, 5,
    0, 10, 10,
    0, 35, 60,
    0, 50, 85,
    0, 75, 100,
    0, 80, 100,
    0, 94, 95,
    0, 96, 80,
    0, 100, 0,
  };

  static void generateToupie(Array<vertex> & roughPart,
			     Array<vertex> & veneerClearPart,
			     Array<vertex> & veneerDarkPart,
			     Array<vertex> & temp)
  {
    const float woodTextureScale = _TV(0.5f);

    // Cône
    temp.empty();
    Mesh::Revolution body(NULL, NULL, toupieData, ARRAY_LEN(toupieData));
    body.generateMesh(temp, _TV(3.f), _TV(2.f), _TV(12), _TV(12));
    Mesh::reprojectTextureXYPlane(temp, woodTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(28.f), _TV(110.f), _TV(10.f));
    glRotatef(_TV(40.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, temp);

    // Axe
    temp.empty();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(temp, _TV(0.3f), _TV(0.3f), _TV(8), _TV(8));
    Mesh::expandPave(temp, _TV(0.f), _TV(1.f), _TV(0.f));
    Mesh::reprojectTextureZYPlane(temp, woodTextureScale);

    glTranslatef(_TV(0.f), _TV(4.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(roughPart, temp);
  }

  static void generateBoat(Array<vertex> & roughPart,
			   Array<vertex> & veneerClearPart,
			   Array<vertex> & veneerDarkPart,
			   Array<vertex> & body)
  {
    body.empty();
    Array<vertex> & mast = Mesh::getTemp();
    Mesh::generatePave(body, _TV(16.f), _TV(2.f), _TV(3.f));
    Mesh::generatePave(mast, _TV(1.f), _TV(1.f), _TV(1.f));

    Array<vertex> & flag = Mesh::getTemp();
    Mesh::generatePave(flag, _TV(3.f), _TV(2.f), _TV(0.05f));
    glLoadIdentity();
    glTranslatef(_TV(-37.f), _TV(12.f), _TV(10.f));
    glRotatef(_TV(20.f), 0, 1.f, 0);
    glPushMatrix();
    glTranslatef(_TV(2.f), _TV(5.5f), _TV(0.f));
    Mesh::applyCurrentProjection(flag);
    glPopMatrix();
    VBO::setupData(VBO::boatFlag, flag DBGARG("boatFlag"));

    for (int i = 0; i < body.size; i++)
    {
      if (body[i].p.y < 0.f)
      {
        body[i].p.x *= _TV(0.5f);
        body[i].u *= _TV(0.5f);
      }
    }

    Mesh::addVerticesUsingCurrentProjection(veneerDarkPart, body);
    glTranslatef(_TV(0.f), _TV(3.f), _TV(0.f));
    glScalef(_TV(1.f), _TV(8.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(roughPart, mast);
  }

  static void addFence(Array<vertex> & house)
  {
    Array<vertex> & cube = Mesh::getTemp();
    Mesh::generatePave(cube, _TV(1.f), _TV(1.f), _TV(1.f));

    glPushMatrix();
    glScalef(_TV(0.5f), _TV(4.f), _TV(0.5f));
    for (int i = 0; i < _TV(14); i++)
    {
      glTranslatef(_TV(3.f), _TV(0.f), _TV(0.f));
      Mesh::addVerticesUsingCurrentProjection(house, cube);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(_TV(11.f), _TV(0.5f), _TV(0.5f));
    glScalef(_TV(21.f), _TV(0.25f), _TV(0.25f));
    Mesh::addVerticesUsingCurrentProjection(house, cube);
    glPopMatrix();
  }

  static float coneRFunc(float t, float theta)
  {
    return _TV(-1.f) * t + _TV(1.001f);
  }

  static void generateWoodHouse(Array<vertex> & roughPart,
				Array<vertex> & veneerClearPart,
				Array<vertex> & veneerDarkPart,
				Array<vertex> & house)
  {
    house.empty();
    Array<vertex> & body = Mesh::getTemp();
    Array<vertex> & roof = Mesh::getTemp();
    Mesh::generatePave(body, _TV(6.f), _TV(4.f), _TV(6.f));
    Mesh::Revolution cone(Mesh::pipeHFunc, coneRFunc);
    cone.generateMesh(roof, _TV(2.5f), _TV(5.f), _TV(1), _TV(-4));
    Mesh::rotate(roof, _TV(45.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(house, body);

    glTranslatef(_TV(0.f), _TV(2.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(house, roof);

    // Ajout de la barrière
    glLoadIdentity();
    glTranslatef(_TV(-12.f), _TV(0.f), _TV(14.f));
    addFence(house);
    glTranslatef(_TV(21.f), _TV(0.f), _TV(0.f));
    glRotatef(_TV(90.f), 0, 1.f, 0);
    addFence(house);

    Mesh::addNoise(house, _TV(0.125f));

    // Placement et ajout à l'ensemble des jouets
    glLoadIdentity();
    glTranslatef(_TV(8.f), _TV(80.f), _TV(-9.f));
    glRotatef(_TV(-5.f), 0, 1.f, 0);
    Mesh::addVerticesUsingCurrentProjection(roughPart, house);
  }

  static void generateTrainSign(Array<vertex> & roughPart)
  {
    glLoadIdentity();
    glTranslatef(_TV(-3.f), _TV(83.f), _TV(-15.f));

    // panneau
    Array<vertex> & sign = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(sign, _TV(1.f), _TV(4.f), _TV(6), _TV(-3));
    Mesh::rotate(sign, _TV(-30.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Mesh::rotate(sign, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::reprojectTextureXYPlane(sign, _TV(0.15f));
    Mesh::translateTexture(sign, _TV(0.5f), _TV(0.43f));
    Mesh::applyCurrentProjection(sign);
    VBO::setupData(VBO::sign, sign DBGARG("sign"));

    Array<vertex> & stick = Mesh::getTemp();
    Mesh::generatePave(stick, _TV(1.f), _TV(4.f), _TV(0.5f));
    Mesh::translate(stick, _TV(0.f), _TV(-4.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(roughPart, stick);
  }


#define LONGUEUR_PLATEAU 120.f
#define LARGEUR_PLATEAU 40.f
#define EPAISSEUR_PLATEAU 2.f

#define LONGUEUR_PIED 180.f
#define LARGEUR_PIED 5.f
#define EPAISSEUR_PIED 2.5f

#define LONGUEUR_TRAVERSE 33.f
#define EPAISSEUR_TRAVERSE 2.f

  void generateMeshes()
  {
    Mesh::clearPool();
    Array<vertex> & roughPart = Mesh::getTemp();
    Array<vertex> & veneerClearPart = Mesh::getTemp();
    Array<vertex> & veneerDarkPart = Mesh::getTemp();

    Array<vertex> & temp = Mesh::getTemp();

    generateBoat(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateStatue(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateHanoi(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateWoodCar(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateRetroSportCar(roughPart, veneerClearPart, veneerDarkPart, temp);
//     generateCanon(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateWoodHouse(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateBilboquet(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateToupie(roughPart, veneerClearPart, veneerDarkPart, temp);
    generateTrainSign(roughPart);

    //
    // FIXME:
    // - finir le canon
    // - finir la voiture de course
    // - camion de pompers avec échelle
    // - camion avec beine
    //

    VBO::setupData(VBO::toysPart1, roughPart DBGARG("toysPart1"));
    VBO::setupData(VBO::toysPart2, veneerClearPart DBGARG("toysPart2"));
    VBO::setupData(VBO::toysPart3, veneerDarkPart DBGARG("toysPart3"));

    // Création des pièces de base
    Array<vertex> & plateau = Mesh::getTemp();
    Array<vertex> & pied = Mesh::getTemp();
    Array<vertex> & traverse = Mesh::getTemp();

    Mesh::generatePave(plateau, LARGEUR_PLATEAU, EPAISSEUR_PLATEAU, LONGUEUR_PLATEAU);
    Mesh::generatePave(pied, EPAISSEUR_PIED, LONGUEUR_PIED, LARGEUR_PIED);
    Mesh::generatePave(traverse, EPAISSEUR_TRAVERSE, EPAISSEUR_TRAVERSE, LONGUEUR_TRAVERSE);

    // Assemblage
    Array<vertex> & shelf = Mesh::getTemp();

    for (unsigned int i = 0; i < 4; ++i)
    {
      // On décale de 58cm et non 60, pour tenir compte de l'épaisseur
      // du pied (sinon il dépasse le plateau)
      glLoadIdentity();
      glTranslatef((0.5f * LONGUEUR_PLATEAU - 1.f) * (i % 2 == 0? 1.f : -1.f),
		   (0.5f * LONGUEUR_PIED),
		   (0.5f * LARGEUR_PLATEAU - 2.f) * (i / 2 == 0? 1.f : -1.f));
      Mesh::addVerticesUsingCurrentProjection(shelf, pied);
    }

    for (unsigned int i = 0; i < 6; ++i)
    {
      glLoadIdentity();
      glTranslatef(0, 10.f + i * 33.f, 0);
      glRotatef(90.f, 0, 1.f, 0); // Pour avoir la texture dans le sens de la longueur
      Mesh::addVerticesUsingCurrentProjection(shelf, plateau);

      glLoadIdentity();
      glTranslatef(-58.8f, 8.f + i * 33.f, 0);
      Mesh::addVerticesUsingCurrentProjection(shelf, traverse);

      glTranslatef(117.6f, 0, 0);
      Mesh::addVerticesUsingCurrentProjection(shelf, traverse);
    }

    Mesh::addTexCoordNoise(shelf, _TV(0.5f));
    VBO::setupData(VBO::shelf, shelf DBGARG("shelf"));
  }
}
