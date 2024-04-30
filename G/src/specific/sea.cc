
#include "hills.hh"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "node.hxx"
#include "renderable.hh"
#include "text.hh"
#include "textureid.hh"
#include "timing.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

namespace Sea
{
  void firstSeagulAnimation(const Node & node, date d)
  {
    float transition = float(d) / float(node.animationStopDate() - node.animationStartDate());
    glTranslatef(transition * _TV(-6.f), transition * _TV(40.f), transition * _TV(-30.f));
  }

  static void createFirstSeagul(Node * root, date startDate, date endDate)
  {
    glLoadIdentity();
    glTranslatef(_TV(13.f), _TV(45.f), _TV(-445.f));
    Node * singleSeagul = Node::New(_TV(21000), _TV(36000));
    Renderable seagulMesh(Material::retroSeagul, VBO_(seagul));
    singleSeagul->attachRenderable(seagulMesh);
    singleSeagul->setAnimation(firstSeagulAnimation);
    singleSeagul->setAnimationStartDate(_TV(32000));
    singleSeagul->attachToNode(root);
  }

  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);
    Renderable sea(Material::sea, VBO_(sea));
    root->attachRenderable(sea);

    createFirstSeagul(root, startDate, endDate);

    glLoadIdentity();
    Node * moving = Node::New(startDate, endDate);
    moving->setAnimation(Hills::translationAnimation);
    moving->attachToNode(root);

    glTranslatef(_TV(-50.f), _TV(2.f), _TV(2200.f));
    glRotatef(_TV(-30.f), 0.f, 1.f, 0.f);
    Renderable boat(Material::floatingBoat, VBO_(boat));
    moving->attachRenderableNode(boat);

    //
    // WARNING:
    // Changer la position du bateau ou les paramètres des vagues va
    // très probablement donner un bateau qui coule !
    //
    // Pour le shader du bateau je voulais utiliser la même fonction
    // que la mer, mais je vois pas de moyen simple de récupérer dans
    // le vertex shader les coordonnées du bateau relatives à la mer.
    //
    // Donc j'ai écrit le shader en dur, en prenant une seule onde et
    // en calant une valeur qui passe à peu près.
    //

    return root;
  }

  static void generateBoat()
  {
    // Yawl:
    //               4      <- h4
    //              /|7     <- h3
    // h2 ->   1   / / \
    //        /|  / /|  \
    // h1 -> 0-2 3-/-5   \
    //         |  6-------9 <- h0
    //        -------+-----
    //        \___________/
    //       ^ ^ ^^  ^    ^
    //      x1 x234  0    x5
    //
    const float h0 = _TV( 0.5f);
    const float h1 = _TV( 1.0f);
    const float h2 = _TV( 5.0f);
    const float h3 = _TV(10.0f);
    const float h4 = _TV(12.0f);

    const float x0 = 0.f;
    const float x1 = _TV(-8.0f);
    const float x2 = _TV(-6.0f);
    const float x3 = _TV(-5.0f);
    const float x4 = _TV(-2.0f);
    const float x5 = _TV( 5.0f);

    const vector2f points[] = {
      vector2f(x1, h1), vector2f(x2, h2), vector2f(x2, h1),   vector2f(x2, h1), // On duplique le dernier
      vector2f(x3, h1), vector2f(x0, h4), vector2f(x0, h1),   vector2f(x0, h1), // Idem
      vector2f(x4, h0), vector2f(x0, h3), vector2f(x5, h0),   vector2f(x5, h0), // Idem
    };
    const float sailAngle = DEG_TO_RAD * _TV(-10.f);

    Mesh::MeshStruct & temp = Mesh::getTempMesh();

    //
    // Voiles
    //
    Mesh::MeshStruct & sails = Mesh::getTempMesh();
    sails.generate2DShape(points, 4, true, true);
    temp.translate(-x2, 0.f, 0.f);
    temp.rotate(sailAngle, 0.f, 0.f, 1.f);
    temp.translate(x2, 0.f, 0.f);
    sails.add(temp);

    temp.clear();
    temp.generate2DShape(points + 4, 4, true, true);
    temp.rotate(sailAngle, 0.f, 0.f, 1.f);
    sails.add(temp);

    temp.clear();
    temp.generate2DShape(points + 8, 4, true, true);
    temp.translate(-x5, 0.f, 0.f);
    temp.rotate(sailAngle, 0.f, 0.f, 1.f);
    temp.translate(x5, 0.f, 0.f);
    sails.add(temp);

    sails.rotate(DEG_TO_RAD * -90.f, 1.f, 0.f, 0.f);
    sails.translate(0.f, _TV(1.f), 0.f);

    //
    // Coque
    //
    Mesh::MeshStruct & body = Mesh::getTempMesh();

    temp.clear();
    Mesh::Pave(_TV(5.f), _TV(2.f), _TV(3.6f)).generateMesh(temp);
    for (int i = 0; i < temp.vertices.size; i++)
    {
      if (temp.vertices[i].p.y < 0.f)
      {
	if (temp.vertices[i].p.x > 0.f) temp.vertices[i].p.x *= _TV(0.2f);
	temp.vertices[i].p.z *= _TV(0.5f);
      }
      if (temp.vertices[i].p.x > 0.f) temp.vertices[i].p.z *= _TV(0.f);
    }
    temp.translate(_TV(2.5f), 0.f, 0.f);
    body.add(temp);

    temp.clear();
    Mesh::Pave(_TV(8.f), _TV(2.f), _TV(3.6f)).generateMesh(temp);
    for (int i = 0; i < temp.vertices.size; i++)
    {
      if (temp.vertices[i].p.y < 0.f)
      {
	if (temp.vertices[i].p.x < 0.f) temp.vertices[i].p.x *= _TV(0.f);
	temp.vertices[i].p.z *= _TV(0.5f);
      }
      if (temp.vertices[i].p.x < 0.f) temp.vertices[i].p.z *= _TV(0.7f);
    }
    temp.translate(-_TV(4.f), 0.f, 0.f);
    body.add(temp);


    Mesh::MeshStruct & boat = Mesh::getTempMesh();
    sails.setColor(Material::beautifulWhite);
    body.setColor(Material::beautifulRed);
    boat.add(sails);
    boat.add(body);

    const float exageration = _TV(2.f); // 1 : voilier de 13 mètres
    for (int i = 0; i < boat.vertices.size; i++)
    {
	boat.vertices[i].p *= exageration;
    }

    boat.computeBaryCoord();

    SETUP_VBO(boat, boat);
  }

  void generateMeshes()
  {
    Mesh::clearPool();
    generateBoat();

    Mesh::MeshStruct sea(655360);
    Mesh::Pave(_TV(1024.f), 0.f, _TV(1024.f)).generateMesh(sea, Mesh::pave_top);
    sea.splitAllFaces(_TV(128));
    sea.setIdPerFace();

    sea.computeBaryCoord();
    sea.setColor(Material::seaColor);

    SETUP_VBO(sea, sea);
  }
}
