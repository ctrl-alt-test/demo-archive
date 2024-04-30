#include "algebra/vector2.hxx"
#include "algebra/vector3.hxx"
#include "array.hxx"
#include "demo.hh"
#include "hills.hh"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "node.hxx"
#include "randomness.hh"
#include "renderable.hh"
#include "snd/midisync.hh"
#include "snd/sound.hh"
#include "texgen/texture.hh"
#include "text.hh"
#include "textureid.hh"
#include "timing.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "balloon.hh"
#include "lighthouse.hh"
#include "seagul.hh"
#include "windturbine.hh"

#include "sys/msys_libc.h"

namespace AllTextures {
  extern Texture::Channel * hillsHMTexture;
}

namespace Hills
{
  void midiAnimation(const Node & node, date d)
  {
    int id = node.visiblePart()[0].id();

    // synchro avec le midi
    float p = Sync::getProgress((Instrument::Id) id, d, _TV(100), _TV(100), _TV(10));
    glTranslatef(p * _TV(0.f), p * _TV(20.f), 0);
    return;

#ifndef DEBUG
      return; // Variables::find non défini
#else
    // synchro avec Usinagaz
    Variable *v = intro.variables.find("ball");
    if (v == NULL) // pas de réseau
      return;

    const float timeScale =  _TV(10.f);
    float maxTime = v->maxTime();
    float time = msys_fmodf((float) d / timeScale, maxTime);
    vector2f vec = v->get2(time) * _TV(0.2f);

    // synchro avec le spectre mp3
    float z = Sound::getIntensity() / _TV(0.1f);
    glTranslatef(vec.x, vec.y, z);
#endif
  }

  static float getHeight(float x, float y)
  {
    // Division par deux, car le terrain fait 1024x1024 et la texture 512x512.
    x /= _TV(2.f);
    y /= _TV(-2.f);

    x += _TV(256.f);
    // Constante sortie du chapeau (trouvée par tatonnements)
    y += _TV(205.f);

    // Constantes de hillsDisplacement.vert
    return (*AllTextures::hillsHMTexture).Bilinear(x, y) * _TV(40.f) + _TV(15.f);
  }

  static void translate(float x, float y, float z)
  {
    glTranslatef(x, y + getHeight(x, z), z);
  }

  static const char * houseShape1[4] = {
    "30  ",
    " 0 2",
    " 0 0",
    "1  0",
  };

  static const char * houseShape2[4] = {
    "0 0 ",
    "0 00",
    "0   ",
    "0  1",
  };

  static const char * houseShape3[4] = {
    " 1 3",
    "   3",
    " 00 ",
    "  00",
  };

  static const char * houseShape4[4] = {
    " 3 3",
    "    ",
    " 30 ",
    "  33",
  };

  static const char * houseShape5[4] = {
    " 3 3",
    "3   ",
    " 33 ",
    "3 33",
  };

  static void createGroupOfHouses(Node* root, const char** shape,
      float gx, float gz, int start)
  {
    // Association chiffre -> mesh
    static VBO::id meshes[] = {VBO_(house), VBO_(THouse), VBO_(church), VBO_(tree1)};
    const int housesLifeSpan = _TV(20000);
    Rand rand((int) (gz * 10.f));

    float angle = rand.fgen(0.f, 360.f);
    for (int z = 0; z < 4; z++) {
      for (int x = 0; x < 4; x++) {
        if (shape[z][x] == ' ')
          continue;
        VBO::id vbo = meshes[shape[z][x] - '0'];
        glPushMatrix();
        glLoadIdentity();
        translate(gx + _TV(12.f) * x + rand.fgenOnAverage(0.f, _TV(0.5f)),
                       _TV(5.f),
                  gz + _TV(-8.f) * z + rand.fgenOnAverage(0.f, _TV(0.5f)));
        float size = rand.fgenOnAverage(_TV(1.f), _TV(0.15f));
        glScalef(size, size, size);
        start += rand.igen(0, _TV(50));
        Node * house = Node::New(start, start + housesLifeSpan);
        house->setAnimation(houseAnimation);
        Renderable houseMesh(Material::retro, vbo);
        house->attachRenderable(houseMesh);
        house->attachToNode(root);
        glPopMatrix();
      }
    }
  }

  static void createLastSeagul(Node * root, date startDate, date endDate)
  {
    glLoadIdentity();
    glTranslatef(_TV(-450.f), _TV(70.f), _TV(25.f));
    Node * singleSeagul = Node::New(_TV(86000), _TV(98000));
    Renderable seagulMesh(Material::retroSeagul, VBO_(seagul));
    singleSeagul->attachRenderable(seagulMesh);
    singleSeagul->setAnimation(lastSeagulAnimation);
    singleSeagul->attachToNode(root);
  }

  static const char treesMatrix[20][21] = {
    "         1         3",
    "14       11         ",
    "    2     2      1  ",
    "                    ",
    "   342        12   2",
    "2   7      2  221   ",
    "        1      1    ",
    "     2  231         ",
    " 12     6731    3   ",
    "  31   1462         ",
    " 1   1 1 2111    3  ",
    "         1          ",
    "     5         31   ",
    "    478    3    2   ",
    "     2 1   2        ",
    "                    ",
    " 2            3    1",
    "      3 1    12     ",
    "    12              ",
    "21          2     12",
  };

  static void createTrees(Node * root, Rand & rand)
  {
    float zshift = _TV(14500.f);
    for (int i = 0; i < _TV(20); i++)
    for (int j = 0; j < _TV(100); j++)
    {
      if (j < _TV(45) && i > _TV(0) && i < _TV(10)) continue;
      if (treesMatrix[j%20][i] == ' ') continue;
      int proba = treesMatrix[j%20][i] - '0';
      float cx = (i - _TV(10)) * _TV(40.f);
      float cz = j * _TV(40.f) + zshift;

      for (int a = 0; a < 3; a++)
        for (int b = 0; b < 3; b++)
        {
          if (rand.igen(10) >= proba) continue;
          float x = cx + _TV(14.f) * a;
          float z = cz + _TV(14.f) * b;
          if (_TV(1))
          {
            x += rand.fgenOnAverage(0.f, _TV(4.f));
            z += rand.fgenOnAverage(0.f, _TV(4.f));
          }
          glLoadIdentity();
          translate(x, _TV(5.f), z);
          VBO::id id = rand.boolean(0.5) ? VBO_(tree1) : VBO_(tree2);
          // Dates différentes pour un chargement progressif
          Node * atree = Node::New(_TV(90000) + j * _TV(50), _TV(120000));
          Renderable tree(Material::retro, id);
          atree->attachRenderable(tree);
          atree->attachToNode(root);
        }
    }
  }

  void addDansingTree(Node * root, Rand & rand)
  {
    VBO::id id = rand.boolean(0.5) ? VBO_(tree1) : VBO_(tree2);
    Node * atree = Node::New(_TV(20000) + rand.igen(1000), _TV(60000));
    Renderable tree(Material::retro, id);
    atree->setAnimation(treeAnimation);
    atree->setAnimationStartDate(rand.igen(_TV(0), _TV(150)));
    atree->attachRenderable(tree);
    atree->attachToNode(root);
  }

  void dansingTrees(Node * root)
  {
    float pos[] = {
      _TV(0.f), _TV(10.f),
      _TV(10.f), _TV(50.f),
      _TV(15.f), _TV(-10.f),
      _TV(20.f), _TV(10.f),
      _TV(25.f), _TV(20.f),
      _TV(-10.f), _TV(15.f),
      _TV(-10.f), _TV(8.f),
      _TV(22.f), _TV(15.f),
      _TV(-4.f), _TV(4.f),
    };

    Rand rand;
    for (int i = 0; i < ARRAY_LEN(pos); i += 2)
    {
      glLoadIdentity();
      translate(pos[i], _TV(5.f), pos[i + 1] + _TV(1500));
      addDansingTree(root, rand);
    }

  const char *smiley[] = {
    _TV("    x x    "),
    _TV("    x x    "),
    _TV("           "),
    _TV(" x       x "),
    _TV("  x     x  "),
    _TV("   xxxxx   "),
  };

  for (int i = 0; i < ARRAY_LEN(smiley); i++)
      for (int j = 0; smiley[i][j] != 0; j++)
      {
        if (smiley[i][j] != 'x') continue;
        glLoadIdentity();
        translate(j * _TV(9.f) + _TV(5.f), _TV(5.f), i * _TV(-9.f) + _TV(4250));
        addDansingTree(root, rand);
      }
  }

  Node * create(date startDate, date endDate)
  {
    assert(AllTextures::hillsHMTexture != NULL);
    Node * root = Node::New(startDate, endDate);
    Rand rand;

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(0.f), _TV(0.f));
    Renderable floor(Material::hills, VBO_(floor));
    root->attachRenderableNode(floor);

    createLastSeagul(root, startDate, endDate);

    if (_TV(0))
      // visualisation des 8 instruments
      for (int i = _TV(0); i < _TV(8); i++)
      {
        glLoadIdentity();
        glTranslatef(_TV(10.f) * i, _TV(50.f), _TV(0.f) * i);
        Node * node = Node::New(worldStartDate, worldEndDate);
        Renderable ball(Material::tunnelLight, VBO_(house));
        node->setAnimation(midiAnimation);
        ball.setId(i);
        node->attachRenderable(ball);
        node->attachToNode(root);
      }

    glLoadIdentity();
    Node * moving = Node::New(startDate, endDate);
    moving->setAnimation(translationAnimation);
    moving->attachToNode(root);

    Text::create(startDate, endDate)->attachToNode(moving);

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(40.f), _TV(450.f));
    Node * seagul = Seagul::create(startDate + _TV(500), startDate + _TV(20000), rand.igen() % 1000);
    seagul->attachToNode(moving);

    glLoadIdentity();
    translate(_TV(35.f), _TV(0.f), _TV(900.f));
    Node * lighthouse = Lighthouse::create(startDate + _TV(15100), endDate);
    lighthouse->attachToNode(moving);

    // Groupes de maisons
    const char** shapes[] = {houseShape1, houseShape2, houseShape3, houseShape4, houseShape5};
    const int housesStartDate = startDate + _TV(17750);
    const float housesZ = _TV(1200.f);
    float z = 0.f;
    for (int i = 0; i < _TV(40); i++) {
      z += rand.fgen(_TV(100.f), _TV(400.f));
      float x = rand.igen(2) == 0 ?
        rand.fgen(_TV(-200.f), _TV(-100.f)) : rand.fgen(_TV(40.f), _TV(200.f));
      int start = housesStartDate + (int) z * _TV(2);
      const char **shape = shapes[rand.igen(0, ARRAY_LEN(shapes) - 1)];
      createGroupOfHouses(moving, shape, x, z + housesZ, start);
    }

    createGroupOfHouses(moving, shapes[0], _TV(0.f), _TV(1250.f), startDate + _TV(16250));

    // Flèche sur le sol
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(40.f), _TV(1620.f));
    Node * arrow = Node::New(startDate + _TV(20200), startDate + _TV(25000));
    arrow->setAnimation(roadAnimation);
    Renderable arrowM(Material::retro, VBO_(arrow));
    arrow->attachRenderable(arrowM);
    arrow->attachToNode(moving);

    // Rangée d'arbres
    glLoadIdentity();
    Node * treeList = Node::New(startDate + _TV(20000), startDate + _TV(30000));
    for (int i = 0; i < _TV(28); i++)
    {
      glLoadIdentity();
      glTranslatef(_TV(-20.f), _TV(38.f), _TV(2150.f) + i * _TV(30));
      addDansingTree(treeList, rand);
      glTranslatef(_TV(15.f), _TV(0.f), _TV(0.f));
      addDansingTree(treeList, rand);
    }
    treeList->attachToNode(moving);

    dansingTrees(moving);

    { // water tower
      glLoadIdentity();
      glTranslatef(_TV(37.f), _TV(35.f), _TV(11700.f));
      Node * tower = Node::New(startDate + _TV(50000), startDate + _TV(61370));
      Renderable towerM(Material::poppingBuilding, VBO_(waterTower));
      tower->attachRenderable(towerM);
      tower->attachToNode(moving);
    }

    // Arbres après le train
    createTrees(moving, rand);

    glLoadIdentity();
    glTranslatef(_TV(-30.f), _TV(50.f), _TV(980.f));
    Balloon::create(startDate, startDate + _TV(20000), Material::retro)->attachToNode(moving);
    glTranslatef(_TV(55.f), _TV(0.f), _TV(80.f));
    Balloon::create(startDate + _TV(600), startDate + _TV(20000), Material::retro)->attachToNode(moving);
    glTranslatef(_TV(-200.f), _TV(40.f), _TV(12500.f));
    Balloon::create(_TV(86370), _TV(95000), Material::retro)->attachToNode(moving);

    // Route
    for (int i = 0; i < _TV(250); i++)
    {
      glLoadIdentity();
      glTranslatef(_TV(-12.f), _TV(37.f), _TV(2200.f) + i * _TV(50));
      int start = startDate + _TV(22800) + i * _TV(80);
      Node * road = Node::New(start, start + _TV(23000));
      Renderable roadMesh(Material::retro, i % 2 == 0 ? VBO_(roadSegment) : VBO_(roadSegment2));
      road->setAnimation(roadAnimation);
      road->attachRenderable(roadMesh);
      road->attachToNode(moving);
    }

    // ------------------------------------------------------------------------
    // Partie train

    // On définit une nouvelle animation pour avoir la position de
    // départ toujours correcte même si on change la vitesse
    date trainStartDate = _TV(86000);
    date trainEndDate = _TV(101870);
    glLoadIdentity();
    Node * fromTrain = Node::New(trainStartDate, trainEndDate);
    fromTrain->setAnimation(translationAnimation);
    fromTrain->attachToNode(root);

    glLoadIdentity();

    for (int j = 0; j < 10; ++j)
      for (int i = 0; i < 3; ++i)
      {
	glLoadIdentity();
	glTranslatef(_TV(-280.f) + _TV(70.f) * i,
		     _TV(30.f),
		     _TV(2200.f) + _TV(160.f) * j);
	glRotatef(_TV(-80.f), 0, 1.f, 0);

	Node * turbine = WindTurbine::create(trainStartDate, trainEndDate, rand.igen() % 2000);
	turbine->attachToNode(fromTrain);
      }
    glLoadIdentity();
    glTranslatef(_TV(-502.f), _TV(35.f), _TV(0.f));
    Renderable posts(Material::retro, VBO_(posts));
    fromTrain->attachRenderableNode(posts);

    return root;
  }

  static void generateWaterTower()
  {
    float towerData[] =
    {
      0, _TV( 00.f), _TV(8.f),
      0, _TV( 32.f), _TV(6.f),
      0, _TV( 45.f), _TV(10.f),
      0, _TV( 46.f), _TV(0.0f),
    };
    initSplineData(towerData, ARRAY_LEN(towerData));

    const int hFaces = _TV(3);
    const int rFaces = _TV(10);

    Mesh::MeshStruct & tower = Mesh::getTempMesh();
    Variable v(towerData, 3, ARRAY_LEN(towerData) / 3, true);
    v.setSpline(_TV(1) ? modeLinear : modeSpline);
    Mesh::Revolution(NULL, NULL, NULL, &v)
      .generateMesh(tower, _TV(1.f), _TV(1.f), hFaces, rFaces);
    tower.setColor(Material::beautifulWhite);
    tower.computeBaryCoord();
    SETUP_VBO(waterTower, tower);
  }

  static void generateHillsMesh()
  {
    Mesh::MeshStruct hills(655360);
    Mesh::Pave(_TV(1024.f), 0.f, _TV(1024.f)).generateMesh(hills, Mesh::pave_top);
    hills.splitAllFaces(_TV(128));
    hills.setIdPerFace();

    hills.computeBaryCoord();
    hills.setColor(Material::hillsColor);
    SETUP_VBO(floor, hills);
  }

  void generateTreeMesh(Mesh::MeshStruct & mesh,
			Mesh::MeshStruct & temp,
			int tFaces, int thetaFaces,
			float height, float diameter,
			float trunkHProportion, float trunkDProportion,
			float topWeight, float bottomWeight,
			float shapeNoise)
  {
    temp.clear();
    Mesh::Revolution(Mesh::sphereHFunc, Mesh::sphereRFunc)
      .generateMesh(temp, 1.f, 0.5f * diameter, tFaces, thetaFaces);
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      vector3f& p = temp.vertices[i].p;
      float y = 0.5f + 0.49f * p.y; // Le 0.49 est là pour éviter le bug en 0 dans msys_powf
      float y1 = msys_powf(y, mix(0.5f, 2.f, topWeight));
      float y2 = 1.f - msys_powf(1.f - y, mix(0.5f, 2.f, bottomWeight));
      p.y = mix(y1, y2, y) * (1.f - trunkHProportion) * height;
    }
    temp.translate(0, trunkHProportion * height, 0);
    temp.addNoise(shapeNoise);
    temp.computeNormals();
    temp.computeBaryCoord();
    mesh.add(temp);
    mesh.setColor(Material::treesColor);

    temp.clear();
    Mesh::Revolution(Mesh::pipeHFunc, Mesh::pipeRFunc)
      .generateMesh(temp, 2.f * trunkHProportion * height, 0.5f * trunkDProportion * diameter, 1, thetaFaces);
    temp.addNoise(shapeNoise);
    temp.computeNormals();
    temp.computeBaryCoord();
    temp.setColor(Material::trunkColor);
    mesh.add(temp);
  }

  static void generateTreeTestMesh()
  {
    Mesh::MeshStruct & trees = Mesh::getTempMesh();

    Mesh::MeshStruct & tree = Mesh::getTempMesh();
    Mesh::MeshStruct & temp = Mesh::getTempMesh();
    for (int j = 0; j < 10; ++j)
      for (int i = 0; i < 10; ++i)
      {
	tree.clear();
	generateTreeMesh(tree, temp, _TV(8), _TV(8), _TV(10.f), _TV(5.f),
          _TV(0.2f), _TV(0.2f), float(i)/9.f, float(j)/9.f, _TV(0.15f));
	tree.translate(_TV(12.f) * i, 0.f, _TV(20.f) * j);
	trees.add(tree);
      }

    SETUP_VBO(tree, trees);
  }

  static void generateTrees()
  {
    Mesh::MeshStruct & tree = Mesh::getTempMesh();
    Mesh::MeshStruct & temp = Mesh::getTempMesh();
    Rand rnd;

    generateTreeMesh(tree, temp, _TV(8), _TV(8), _TV(10.f), _TV(5.f),
      _TV(0.2f), _TV(0.2f), rnd.fgen(0.f, 1.f), rnd.fgen(0.f, 1.f), _TV(0.15f));
    SETUP_VBO(tree1, tree);

    tree.clear();
    generateTreeMesh(tree, temp, _TV(8), _TV(8), _TV(10.f), _TV(5.f),
      _TV(0.2f), _TV(0.2f), rnd.fgen(0.f, 1.f), rnd.fgen(0.f, 1.f), _TV(0.15f));
    SETUP_VBO(tree2, tree);
  }

  static void generateHouseMainMesh(Mesh::MeshStruct & mesh,
				    float length, float width,
				    float wallHeight, float roofHeight,
				    float leftRoofSlant, float rightRoofSlant)
  {
    const Mesh::PaveFaces wallFaces = (Mesh::PaveFaces)(Mesh::pave_left | Mesh::pave_right | Mesh::pave_back | Mesh::pave_front);

    Mesh::Pave(length, roofHeight, width).generateMesh(mesh, wallFaces);
    for (int i = 0; i < mesh.vertices.size; i++)
      if (mesh.vertices[i].p.y > 0)
      {
	mesh.vertices[i].p.z = 0;
	if (mesh.vertices[i].p.x > 0)
	{
	  mesh.vertices[i].p.x -= leftRoofSlant;
	}
	else
	{
	  mesh.vertices[i].p.x += rightRoofSlant;
	}
      }

    mesh.translate(0, 0.5f * (wallHeight + roofHeight), 0);
    Mesh::Pave(length, wallHeight, width).generateMesh(mesh, wallFaces);
    mesh.translate(0, 0.5f * wallHeight, 0);

    mesh.setColor(Material::beautifulWhite);
  }

  static void paintHouse(Array<vertex> & house)
  {
    for (int i = 0; i < house.size; i++)
    {
      const vector3f color =
	(house[i].n.y > 0.f ? Material::roofsColor : Material::beautifulWhite);

      house[i].r = color.x;
      house[i].g = color.y;
      house[i].b = color.z;
    }
  }

  static void generateHouseMesh()
  {
    const float length = _TV(10.f);
    const float width = _TV(6.f);
    const float wallHeight = _TV(3.f);
    const float roofHeight = _TV(3.f);

    Mesh::MeshStruct & house = Mesh::getTempMesh();
    generateHouseMainMesh(house, length, width, wallHeight, roofHeight, 0.f, 0.f);
    house.computeNormals();
    house.computeBaryCoord();
    paintHouse(house.vertices);

    SETUP_VBO(house, house);
  }

  static void generateTHouseMesh()
  {
    const float length = _TV(12.f);
    const float subLength = _TV(3.f);
    const float width = _TV(6.f);
    const float wallHeight = _TV(3.f);
    const float roofHeight = _TV(3.f);

    Mesh::MeshStruct & main = Mesh::getTempMesh();
    generateHouseMainMesh(main, length, width, wallHeight, roofHeight, 0.f, 0.f);

    Mesh::MeshStruct & sub = Mesh::getTempMesh();
    generateHouseMainMesh(sub, subLength, width, wallHeight, roofHeight, -roofHeight, roofHeight);
    sub.rotate(DEG_TO_RAD * 90.f, 0.f, 1.f, 0.f);
    sub.translate(0.f, 0.f, 0.5f * (width + subLength));

    Mesh::MeshStruct & house = Mesh::getTempMesh();
    house.add(main);
    house.add(sub);

    house.computeNormals();
    house.computeBaryCoord();
    paintHouse(house.vertices);

    SETUP_VBO(THouse, house);
  }

  static void generateChurchMesh()
  {
    const float length = _TV(10.f);
    const float width = _TV(6.f);
    const float wallHeight = _TV(5.f);
    const float roofHeight = _TV(4.f);
    const float roofSlant = _TV(2.f);

    const float towerWidth = _TV(4.f);
    const float towerHeight = _TV(10.f);
    const float spikeHeight = _TV(6.f);

    Mesh::MeshStruct & main = Mesh::getTempMesh();
    generateHouseMainMesh(main, length, width, wallHeight, roofHeight, roofSlant, 0.f);

    const Mesh::PaveFaces wallFaces = (Mesh::PaveFaces)(Mesh::pave_left | Mesh::pave_right | Mesh::pave_back | Mesh::pave_front);

    Mesh::MeshStruct & tower = Mesh::getTempMesh();
    Mesh::Pave(towerWidth, spikeHeight, towerWidth).generateMesh(tower, wallFaces);
    for (int i = 0; i < tower.vertices.size; i++)
    {
      if (tower.vertices[i].p.y > 0)
      {
	tower.vertices[i].p.x = 0;
	tower.vertices[i].p.z = 0;
      }
    }
    tower.translate(0, 0.5f * (towerHeight + spikeHeight), 0);
    Mesh::Pave(towerWidth, towerHeight, towerWidth).generateMesh(tower, wallFaces);
    tower.translate(0, 0.5f * towerHeight, 0);

    Mesh::MeshStruct & church = Mesh::getTempMesh();
    church.add(main);
    church.translate(0.5f * (length + towerWidth), 0, 0);
    church.add(tower);

    church.computeNormals();
    church.computeBaryCoord();
    paintHouse(church.vertices);

    SETUP_VBO(church, church);
  }

  static void generatePostsMesh()
  {
    const float height = _TV(20.f);
    const float width = _TV(1.2f);

    // Poteaux synchronisés avec la caisse claire
    const float totalDistance = _TV(3450.f);
    const int numberOfPosts = _TV(14);

    Mesh::MeshStruct & posts = Mesh::getTempMesh();
    Mesh::MeshStruct & post = Mesh::getTempMesh();
    for (int i = 0; i < numberOfPosts; ++i)
    {
      post.clear();
      Mesh::Pave(width, height, width).generateMesh(post);
      post.translate(0, 0.5f * height, _TV(200.f) + (i * totalDistance)/numberOfPosts);
      posts.add(post);
    }

    posts.setColor(Material::beautifulBlack);
    SETUP_VBO(posts, posts);
  }

  static void generateArrow()
  {
    const vector2f points[] = {
      vector2f(_TV(0.0f),  _TV(40.0f)),
      vector2f(_TV(10.0f),  _TV(20.0f)),
      vector2f(_TV(-10.0f), _TV(20.0f)),
      vector2f(_TV(0.0f),  _TV(40.0f)),

      vector2f(_TV(5.0f),  _TV(20.0f)),
      vector2f(_TV(5.0f),  _TV(0.0f)),
      vector2f(_TV(-5.0f), _TV(0.0f)),
      vector2f(_TV(-5.0f), _TV(20.0f)),
    };

    Mesh::MeshStruct & arrow = Mesh::getTempMesh();
    arrow.generate2DShape(points, ARRAY_LEN(points), true, _TV(1) == 1);
    arrow.rotate(DEG_TO_RAD * _TV(0.f), _TV(1.f), _TV(0.f), _TV(0.f));
    if (_TV(0)) arrow.computeNormals();
    if (_TV(1)) arrow.computeBaryCoord();
    SETUP_VBO(arrow, arrow);
  }

  void generateMeshes()
  {
    Mesh::clearPool();

    Mesh::MeshStruct & road = Mesh::getTempMesh();
    Mesh::Pave(_TV(7.f), _TV(4.f), _TV(50.f)).generateMesh(road);
    road.setColor(Material::road1Color);
    SETUP_VBO(roadSegment, road);
    road.setColor(Material::road2Color);
    SETUP_VBO(roadSegment2, road);

    generateHillsMesh();
    generateTreeTestMesh();
    generateWaterTower();
    generateTrees();
    generateHouseMesh();
    generateTHouseMesh();
    generateChurchMesh();
    generatePostsMesh();
    generateArrow();
  }

#undef _TV
#define _TV(x) x

  void translationAnimation(const Node & node, date d)
  {
    date t0 = node.animationStartDate();
    date t1 = t0 + d;
    float pos_t0 = Demo::getTranslatedPos(t0);
    float pos_t1 = Demo::getTranslatedPos(t1);
    float dist = pos_t1 - pos_t0;
    glTranslatef(0.f, 0.f, dist);
  }

  void roadAnimation(const Node & node, date d)
  {
    float y = 1.f - smoothStep(0, 1500, d);
    glTranslatef(0.f, y * 80.f, 0.f);
  }

  void lastSeagulAnimation(const Node & node, date d)
  {
    float z = d * _TV(-0.02f);
    glTranslatef(0.f, 0.f, z);
  }

  void houseAnimation(const Node & node, date d)
  {
    int topTime = _TV(1000); // temps correspondant au sommet de la parabole
    float top = _TV(30.f);  // hauteur du saut
    float t = (float) d - topTime;
    float y = (t * t) * _TV(-0.0003f) + top;
    if (t > 0.f) y = msys_max(y, 0.f);
    glTranslatef(0.f, y, 0.f);
  }

  void treeAnimation(const Node & node, date d)
  {
    float p = msys_sinf(d * _TV(0.005f)) * _TV(25.f);
    glRotatef(p, _TV(0.f), _TV(0.f), _TV(1.f));
  }
}
