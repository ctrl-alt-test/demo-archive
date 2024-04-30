#include "algebra/vector2.hxx"
#include "algebra/vector3.hxx"
#include "city.hh"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/building.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "node.hxx"
#include "randomness.hh"
#include "renderable.hh"
#include "text.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "specific/hills.hh"

#include "sys/msys_glext.h"

namespace City
{
  const int blockMaxWidth = 9; // 8 + 1 pour la rue
  const int blockMaxLength = 5; // 4 + 1 pour la rue
  const int blocksPerAvenue = 4; // Une avenue tous les 4 blocks

  const int cityWidth = 16 * blockMaxWidth + 16 / blocksPerAvenue;
  const int cityLength = 8 * blockMaxLength + 8 / blocksPerAvenue;
  const float cellSize = 15.f;

  const int numberOfCityParts = 7; // VBO_(city7) - VBO::city1 + 1;

  // ==========================================================================================
  // Coordonnées de la grille
  vector2f getCityCoordinates(const vector2f & position)
  {
    return vector2f(position.x * cityWidth * cellSize,
		    position.y * cityLength * cellSize);
  }

  // ==========================================================================================
  // Déformation de la grille

  vector2f getWarpedCoordinates(const vector2f & v, const float * weights)
  {
    float y0 = mix(weights[0], weights[1], v.x);
    float y1 = mix(weights[2], weights[3], v.x);
    float x0 = mix(weights[0], weights[2], v.y);
    float x1 = mix(weights[1], weights[3], v.y);

    return vector2f(hermite(x0, x1, v.x),
		    hermite(y0, y1, v.y));
  }

  // position est une coordonnée 2D dans [0..1]x[0..1]
  //
  // w00, w01, w10 et w11 sont des coeffs à choisir entre 0 et 3,
  // pour les quatre angles de la zone 2D
  //    0: dérivée nulle, les points sont éloignés
  //    1: dérivée à 1, l'éloignement des points ne change pas
  //    3: dérivée max, les points sont proches (au delà de 3 ce n'est plus monotone)
  //
  // newPosition, newX et newY sont le repère après déformation locale
  //
  // Exemple de déformation:
  // https://www.shadertoy.com/view/4sS3Wc
  //
  void getWarpedTransform(const vector2f & position, const float * weights,
			  vector2f & newPosition, vector2f & newX, vector2f & newY)
  {
    const float d = 0.01f;
    const vector2f dx(d, 0);
    const vector2f dy(0, d);
    newPosition = getWarpedCoordinates(position, weights);
    newX = (getWarpedCoordinates(position + dx, weights) - newPosition) / d;
    newY = (getWarpedCoordinates(position + dy, weights) - newPosition) / d;
  }

  void putSubMeshInPlace(Array<vertex> & vertices, int start, int end,
			 const vector2f & position, const float * weights, bool bend = true)
  {
    vector2f newPosition, newX, newY;
    getWarpedTransform(position, weights, newPosition, newX, newY);

    const vector2f t = getCityCoordinates(newPosition);
    for (int i = start; i < end; ++i)
    {
      vector3f & p = vertices[i].p;
      if (bend)
      {
	p.x = newX.x * p.x + newY.x * p.z;
	p.z = newX.y * p.x + newY.y * p.z;
      }
      p.x += t.x;
      p.z += t.y;
    }
  }

  void setMeshColor(Array<vertex> & vertices, int start, int end, const vector3f & color)
  {
    for (int i = start; i < end; ++i)
    {
      vertices[i].r = color.x;
      vertices[i].g = color.y;
      vertices[i].b = color.z;
    }
  }

  void arrowAnimation(const Node & node, date d)
  {
    const float t = interpolate(d, _TV(0), _TV(1000));

    // Imitation de rebond:
    const float x = (t*t*t - t*t) * (_TV(0.3f) * sin(_TV(20.f) * t)) + msys_min(1.f, _TV(2.f) * t);
    //              ^^^^^^^^^^^^^    ^^^^^^^^        ^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^^^
    //              mix pente/sin    amplitude       frequence         pente lineaire, jusqu'à 1

    glTranslatef(_TV(0.f) * x, _TV(0.f) * x, _TV(100.f) * x);
  }

  // ==========================================================================================
  // Buildings

  float height(float a, float b)
  {
    float x = (mix(_TV(0.5f), _TV(0.0f), a) +
	       mix(_TV(0.0f), _TV(0.5f), b));
    float xn = msys_powf(x, _TV(3.f));
    return mix(_TV(15.f), _TV(120.f), xn);
  }

  void addBuilding1(Mesh::MeshStruct & block, Rand & rand,
		    float a, float b, float width, float length)
  {
    const float averageHeight = height(a, b);

    const float Astyle = mix(_TV(0.3f), _TV(0.9f), a*a);
    const float Bstyle = mix(_TV(0.f), _TV(0.7f), b);
    const vector2f style(Astyle, Bstyle);

    Mesh::Building(averageHeight, width, length, style)
      .generateMesh(block, 0.f, 1.f, _TV(0.25f), rand, Mesh::Building::maxDetail);
  }

  void addBuilding2(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand,
		    float a, float b, float width, float length)
  {
    const float averageHeight = height(a, b);

    temp.clear();
    const float baseHeight = averageHeight * _TV(0.9f);
    const float roofHeight = averageHeight * _TV(0.1f);
    const float baseWidth =  width * rand.fgen(_TV(0.7f), _TV(0.95f));
    const float baseLength = length * rand.fgen(_TV(0.7f), _TV(0.95f));
    Mesh::Pave(baseWidth, baseHeight, baseLength)
      .generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
    temp.translate(0.f, baseHeight * 0.5f, 0.f);
    block.add(temp);

    if (!rand.boolean(_TV(0.4f)))
    {
      temp.clear();

      if (width > 2.f * cellSize ||
	  length > 2.f * cellSize ||
	  rand.boolean(_TV(0.9f)))
      {
	// Toit oblique
	Mesh::Pave(baseWidth, 2.f * roofHeight, baseLength).generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
	bool roofOrientation = rand.boolean(0.5f);
	for (int i = 0; i < temp.vertices.size; i++) {
	  temp.vertices[i].p.y = msys_max(temp.vertices[i].p.y, 0.f);
	  if (roofOrientation && temp.vertices[i].p.x < 0.f) temp.vertices[i].p.y *= _TV(0.2f);
	  if (!roofOrientation && temp.vertices[i].p.z < 0.f) temp.vertices[i].p.y *= _TV(0.2f);
	}
	temp.translate(0.f, baseHeight, 0.f);
      }
      else
      {
	// Toit à quatre côtés
	Mesh::Pave(baseWidth, _TV(5.f) * roofHeight, baseLength).generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_top & ~Mesh::pave_bottom));
	for (int i = 0; i < temp.vertices.size; i++)
	  if (temp.vertices[i].p.y > 0)
	  {
	    temp.vertices[i].p.x = 0;
	    temp.vertices[i].p.z = 0;
	  }
	  else
	  {
	    temp.vertices[i].p.y = 0;
	  }
	temp.translate(_TV(0.f), baseHeight, _TV(0.f));
      }
      block.add(temp);
    }
  }

  void addBuilding(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand,
		   float a, float b, float width, float length)
  {
    if (rand.boolean(1.f - clamp(a, _TV(0.15f), _TV(0.9f))))
      addBuilding1(block, rand, a, b, width, length);
    else
      addBuilding2(block, temp, rand, a, b, width, length);
  }

  vector3f buildingColor(int seed)
  {
    Rand rand(seed);

    const vector3f colors[] = {
      vector3f(_TV(1.00f), _TV(1.00f), _TV(1.00f)), // Blanc
      vector3f(_TV(0.10f), _TV(0.23f), _TV(0.44f)), // Gris sombre
      vector3f(_TV(0.25f), _TV(0.35f), _TV(0.45f)), // Brique
    };
    const float w1 = rand.fgen() + 0.01f;
    const float w2 = rand.fgen() + 0.01f;
    const float w3 = rand.fgen() + 0.01f;
    const float sum = w1 + w2 + w3;
    return (w1 * colors[0] + w2 * colors[1] + w3 * colors[2]) / sum;
  }

  // ==========================================================================================
  // Blocs
  // Un bloc correspond à 8x4 cellules

  void generateStandardBlock(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand, float a, float b,
			     const vector2f & position, const float * weights)
  {
    glLoadIdentity();

    // Les petits bâtiments font 2x2 cellules, on peut donc en mettre 4x2.
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 2; j++)
      {
	const int start = block.vertices.size;
	const vector2f dp = vector2f(float(2 * i - _TV(3)) / cityWidth, float(2 * j - _TV(1)) / cityLength);

	addBuilding(block, temp, rand, a, b, 2.f * cellSize, 2.f * cellSize);
	putSubMeshInPlace(block.vertices, start, block.vertices.size, position + dp, weights);
	setMeshColor(block.vertices, start, block.vertices.size, buildingColor(rand.seed() + i + 4 * j));
      }
  }

  void generateBlockWithSingleBuilding(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand, float a, float b,
				       const vector2f & position, const float * weights)
  {
    int start = block.vertices.size;
    if (rand.boolean(_TV(0.3f)))
    {
      //  Un seul, très gros bâtiment
      addBuilding1(block, rand, a, b, 8.f * cellSize, 4.f * cellSize);
      putSubMeshInPlace(block.vertices, start, block.vertices.size, position, weights, false);
    }
    else
    {
      //  Ou deux gros
      addBuilding(block, temp, rand, a, b, 4.f * cellSize, 4.f * cellSize);
      putSubMeshInPlace(block.vertices, start, block.vertices.size, position - vector2f(2.f / cityWidth, 0), weights, false);

      int start = block.vertices.size;
      addBuilding(block, temp, rand, a, b, 4.f * cellSize, 4.f * cellSize);
      putSubMeshInPlace(block.vertices, start, block.vertices.size, position + vector2f(2.f / cityWidth, 0), weights, false);
    }
  }

  void generateBlockWithEllipseBuilding(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand, float a, float b,
					const vector2f & position, const float * weights)
  {
    temp.clear();
    Mesh::Pave(_TV(6.f), _TV(3.f), _TV(16.5f))
      .generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
    temp.translate(0.f, _TV(15.f), 0.f);

    Mesh::Revolution(Mesh::wheelHFunc, Mesh::wheelRFunc)
      .generateMesh(temp, _TV(30.f), _TV(25.f), _TV(9), _TV(10));
    temp.expandPave(_TV(25.f), _TV(10.f), _TV(0.f));
    for (int i = 0; i < temp.vertices.size; i++)
      temp.vertices[i].p.y = msys_max(temp.vertices[i].p.y, 0.f);

    putSubMeshInPlace(temp.vertices, 0, temp.vertices.size, position, weights, false);
    block.add(temp);
  }

  static void addTree(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, int seed,
		      const vector2f & position, const float * weights)
  {
    Rand rand(seed);
    float topWeight = rand.fgen(0.f, 1.f);
    float bottomWeight = rand.fgen(0.f, 1.f);
    Mesh::MeshStruct tree(500);
    Hills::generateTreeMesh(tree, temp, _TV(8), _TV(8), _TV(10.f), _TV(5.f),
          _TV(0.2f), _TV(0.2f), topWeight, bottomWeight, _TV(0.15f));
    tree.translate(_TV(0.f), _TV(0.f), _TV(0.f));
    putSubMeshInPlace(tree.vertices, 0, tree.vertices.size, position, weights, false);
    block.add(tree);
  }

  static void addTrees(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, int seed,
		       const vector2f & position, const float * weights)
  {
    Rand rand(seed);
    const int treesPerCell = _TV(1);
    for (int j = 0; j < 4 * treesPerCell; ++j)
      for (int i = 0; i < 8 * treesPerCell; ++i)
	if (rand.boolean(0.3f))
	{
	  const vector2f dp = vector2f((rand.fgen(float(i + 0.25f), float(i + 0.75f)) - _TV(4.f)) / cityWidth,
				       (rand.fgen(float(j + 0.25f), float(j + 0.75f)) - _TV(2.f)) / cityLength);
	  addTree(block, temp, rand.seed(), position + dp, weights);
	}
  }

  void generateBlock(Mesh::MeshStruct & block, Mesh::MeshStruct & temp, Rand & rand, float a, float b,
		     const vector2f & position, const float * weights)
  {
    // Le trottoir
    const float walkWaySize = _TV(6.f);
    Mesh::Pave(8.f * cellSize + walkWaySize, _TV(0.25f), 4.f * cellSize + walkWaySize)
      .generateMesh(block, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));

    block.translate(0, _TV(-0.125f), 0);
    putSubMeshInPlace(block.vertices, 0, block.vertices.size, position, weights);

    // Couleur du trottoir
    block.setColor(vector3f(_TV(0.3f), _TV(0.4f), _TV(0.5f)));

    const int start = block.vertices.size;
    float r = rand.fgen(0.f, _TV(10.f));
    if (r < _TV(0.4f) && a > _TV(0.1f) && a < _TV(0.8f) && b > _TV(0.f))
    {
      // Attention en réglant les conditions de rand : apparemment le
      // random n'a pas une entropie terrible, et avoir deux de ces
      // bâtiments l'un à côté de l'autre fait bizarre.
      generateBlockWithEllipseBuilding(block, temp, rand, a, b, position, weights);
      setMeshColor(block.vertices, start, block.vertices.size, buildingColor(rand.seed()));
    }
    else if (r < _TV(2.f))
    {
      generateBlockWithSingleBuilding(block, temp, rand, a, b, position, weights);
      setMeshColor(block.vertices, start, block.vertices.size, buildingColor(rand.seed()));
    }
    else if (r < _TV(2.5f))
      {
	// parc
	block.setColor(Material::hillsColor);
        addTrees(block, temp, rand.seed(), position, weights);
      }
    else
    {
      generateStandardBlock(block, temp, rand, a, b, position, weights);
    }
  }

  // ==========================================================================================
  // City patch

  void generateCity(Mesh::MeshStruct & city, int seed, const float * weights, const float * style)
  {
    Mesh::MeshStruct & temp = Mesh::getTempMesh();
    Mesh::MeshStruct & blockMesh = Mesh::getTempMesh();

    Mesh::Pave(cellSize * cityWidth, 0.f, cellSize * cityLength).generateMesh(city, Mesh::pave_top);
    city.translate(0, _TV(-0.25f), 0);

    // Couleur des rues
    city.setColor(vector3f(_TV(0.2f), _TV(0.3f), _TV(0.4f)));

    // Il y a un bug avec le fog pour les très grand polys, en
    // attendant de corriger, on découpe.
    city.splitAllFacesDownToMaxSide(_TV(200.f));

    Rand rand(seed);
    int cy = _TV(0); // pour placer les avenues
    for (int j = 0; j < cityLength; j += blockMaxLength)
    {
      int cx = _TV(0);
      for (int i = 0; i < cityWidth; i += blockMaxWidth)
      {
	float x = float(i) / float(cityWidth - 1);
	float y = float(j) / float(cityLength - 1);
	const vector2f position(x, y);
	x = 2.f * x - 1.f;

	const float a1 = x < 0 ? smoothMix(style[0], style[1],  x + 1.f) :  smoothMix(style[1], style[2],   x);
	const float b1 = x < 0 ? smoothMix(style[3], style[4],  x + 1.f) :  smoothMix(style[4], style[5],   x);
	const float a2 = x < 0 ? smoothMix(style[6], style[7],  x + 1.f) :  smoothMix(style[7], style[8],   x);
	const float b2 = x < 0 ? smoothMix(style[9], style[10], x + 1.f) :  smoothMix(style[10], style[11], x);
	const float a = clamp(smoothMix(a1, a2, y) + rand.fgenOnAverage(0.f, _TV(0.1f)));
	const float b = smoothMix(b1, b2, y);

	blockMesh.clear();
        generateBlock(blockMesh, temp, rand, a, b, position, weights);

	blockMesh.translate((_TV(5.0f) - 0.5f * cityWidth) * cellSize, 0,
			    (_TV(2.5f) - 0.5f * cityLength) * cellSize);
        city.add(blockMesh);

	if (++cx % blocksPerAvenue == 0) i++;
      }

      if (++cy % blocksPerAvenue == 0) j++;
    }
    city.computeBaryCoord();
    city.computeNormals();
  }

  // ==========================================================================================

  // Une réplique de la célèbre statue d'Incubation.
  static void generateStatue(Mesh::MeshStruct & statue)
  {
    Mesh::MeshStruct & temp = Mesh::getTempMesh();
    Mesh::Revolution rev(Mesh::wheelHFunc, Mesh::wheelRFunc);

    const float x = _TV(-50.f);
    const float y = _TV(45.f);
    const float z = _TV(5.f);

    const float scale = _TV(2.f);
    const float woodTextureScale = _TV(0.1f);

    // Socle
    rev.generateMesh(temp, _TV(0.5f), _TV(0.25f), _TV(7), _TV(-8));
    temp.rotate(DEG_TO_RAD * 180.f / _TV(8.f), 0.f, 1.f, 0.f);

    temp.expandPave(_TV(1.8f) * scale, _TV(0.8f), _TV(1.8f) * scale);
    temp.translate(x, y, z);
    statue.add(temp);

    // Gros cubes
    temp.clear();
    rev.generateMesh(temp, _TV(0.25f), _TV(0.125f), _TV(7), _TV(-8));
    temp.rotate(DEG_TO_RAD * 180.f / _TV(8.f), 0, 1.f, 0);

    temp.expandPave(scale, scale, scale);
    temp.translate(x, y + _TV(1.6f) * scale, z);
    statue.add(temp);

    temp.translate(0, _TV(4.35f) * scale, 0);
    statue.add(temp);

    // Petit cube
    temp.clear();
    rev.generateMesh(temp, _TV(0.25f), _TV(0.125f), _TV(7), _TV(-8));
    temp.rotate(DEG_TO_RAD * 180.f / _TV(8.f), 0, 1.f, 0);

    temp.expandPave(0.6f * scale, 0.6f * scale, 0.6f * scale);
    temp.rotate(DEG_TO_RAD * 45.f, 1.f, 0.f, 1.f);
    temp.translate(x, y + _TV(3.8f) * scale, z);

    statue.add(temp);
    statue.computeNormals();
    statue.computeBaryCoord();
  }

  void generateMeshes()
  {
    Mesh::clearPool();

    Mesh::MeshStruct & statue = Mesh::getTempMesh();
    generateStatue(statue);
    statue.scale(_TV(1.2f), _TV(1.2f), _TV(1.2f));
    SETUP_VBO(statue, statue);

    const float cityWarpWeigths[] = {
      _TV(0.9f), _TV(1.0f),
      _TV(1.1f), _TV(0.8f),
      _TV(0.8f), _TV(1.0f),
      _TV(0.7f), _TV(1.0f),
      _TV(1.1f), _TV(0.9f),
      _TV(0.95f), _TV(1.05f),
      _TV(1.0f), _TV(1.0f),
      _TV(1.0f), _TV(1.0f),
    };
    assert(ARRAY_LEN(cityWarpWeigths) == 2 * (numberOfCityParts + 1));

    // Style fonction de a et b
    // a est un peu un indicateur du niveau de pauvrete
    // b est un peu un indicateur de hauteur
    //
    //     0              b ^                  1
    //    1                 |  Petites tours
    //       Grandes tours, |  d'appartements
    //         quartier     |    moches de
    //       des affaires.  |    banlieue.
    //                      |                  a
    //      ----------------+------------------->
    //                      |
    //       Petites tours, |   Bâtiments de
    //         bureaux ou   |  banlieue, plus
    //       résidences de  |   larges que
    //    0    standing.    |     hauts.
    //
    const float cityStyle[] = {
      //              A                                  B
      // gauche    centre     droite       gauche     centre     droite
      _TV(0.8f), _TV(1.0f), _TV(0.8f),   _TV(0.8f), _TV(0.5f), _TV(0.8f),
      _TV(1.0f), _TV(1.0f), _TV(1.0f),   _TV(1.0f), _TV(0.2f), _TV(0.0f),
      _TV(0.6f), _TV(0.8f), _TV(0.0f),   _TV(1.0f), _TV(0.5f), _TV(0.8f),
      _TV(0.4f), _TV(0.4f), _TV(0.4f),   _TV(0.5f), _TV(0.3f), _TV(0.4f),
      _TV(0.5f), _TV(0.2f), _TV(0.4f),   _TV(1.0f), _TV(0.1f), _TV(0.2f),
      _TV(0.2f), _TV(0.1f), _TV(0.2f),   _TV(0.4f), _TV(0.2f), _TV(0.3f),
      _TV(0.0f), _TV(0.0f), _TV(0.0f),   _TV(0.0f), _TV(0.4f), _TV(0.0f),
      _TV(0.0f), _TV(0.0f), _TV(0.0f),   _TV(0.1f), _TV(1.0f), _TV(0.1f),
    };
    assert(ARRAY_LEN(cityStyle) == 6 * (numberOfCityParts + 1));

    Mesh::MeshStruct city(200000);

    VBO::id cityId = NEW_VBOS(city, numberOfCityParts);
    for (int i = 0; i < numberOfCityParts; i++)
    {
      const int seed = i * cityWidth * cityLength;
      city.clear();
      generateCity(city, seed, cityWarpWeigths + 2 * i, cityStyle + 6 * i);
      VBO::id id = (VBO::id) (cityId + i);
      VBO::setupData(id, city.vertices DBGARG("city"));
    }
  }

  static void createFinalSeagul(Node * root)
  {
    glLoadIdentity();
    Node * singleSeagul = Node::New(_TV(282500), _TV(305000));
    Renderable seagulMesh(Material::retroSeagul, VBO_(seagul));
    seagulMesh.setId(-1); // cf. retro.frag
    singleSeagul->attachRenderable(seagulMesh);
    singleSeagul->setAnimation(lastSeagulAnimation);
    singleSeagul->attachToNode(root);
  }

  static void createFinalSeagulGroup(Node * root)
  {
    glLoadIdentity();
    Node * seagulGroup = Node::New(_TV(282500), _TV(305000));
    Renderable seagulMesh(Material::retroSeagul, VBO_(seagulGroup));
    seagulMesh.setId(-1); // cf. retro.frag
    seagulGroup->attachRenderable(seagulMesh);
    seagulGroup->setAnimation(lastSeagulGroupAnimation);
    seagulGroup->attachToNode(root);
  }

  static void createStatue(Node * root)
  {
    glLoadIdentity();
    glTranslatef(_TV(860.f), _TV(-53.f), _TV(3991.f));
    Renderable statueMesh(Material::retro, VBO_(statue));
    root->attachRenderableNode(statueMesh);
  }

  static void createArrow(Node * root, date d, float angle, float x, float z)
  {
    glLoadIdentity();
    glTranslatef(x, _TV(4.f), z);
    glRotatef(angle, _TV(0.f), _TV(1.f), _TV(0.f));
    glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    glScalef(_TV(0.25f), _TV(0.25f), _TV(0.25f));
    Node * arrow = Node::New(d, d + _TV(5000));
    arrow->setAnimation(arrowAnimation);
    Renderable arrowM(Material::retro, VBO_(arrow));
    arrowM.setId(-1); // cf. retro.frag
    arrow->attachRenderable(arrowM);
    arrow->attachToNode(root);
  }

  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);
    Text::create(startDate, endDate)->attachToNode(root);

    createFinalSeagul(root);
    createFinalSeagulGroup(root);
    createStatue(root);
    createArrow(root, _TV(256000), _TV(90.f), _TV(-694.f), _TV(316.f));
    createArrow(root, _TV(259000), _TV(0.f),  _TV(102.f),   _TV(302.f));
    createArrow(root, _TV(262000), _TV(90.f), _TV(-95.f),  _TV(1110.f));

    const date dates[] = {
      //       Création, destruction
      startDate +     0, startDate +  7000,
      startDate +  3000, startDate + 14000,
      startDate + 10000, startDate + 21000,
      startDate + 17000, startDate + 28000,
      startDate + 21000, startDate + 35000,
      startDate + 28000, startDate + 42000,
      startDate + 35000, startDate + 49000,
    };
    assert(ARRAY_LEN(dates) == 2 * numberOfCityParts);

    VBO::id cityId = VBO_(city);
    for (int i = 0; i < numberOfCityParts; i++)
    {
      glLoadIdentity();
      glTranslatef(i * _TV(0.f) * cityWidth * cellSize, 0.f,
		   i * _TV(1.f) * cityLength * cellSize);
      Renderable cityPatch(Material::retro, (VBO::id)(cityId + i));

      //Node * n = Node::New(dates[2 * i], dates[2 * i + 1]);
      Node * n = Node::New(startDate, endDate);
      n->attachRenderable(cityPatch);
      n->attachToNode(root);
    }
    return root;
  }

  void lastSeagulAnimation(const Node & node, date d)
  {
    VAR(lastSeagul)->setSpline(modeLinear);
    vector3f pos = VAR(lastSeagul)->get3((float) d);
    glTranslatef(pos.x, pos.y, pos.z);
  }

  void lastSeagulGroupAnimation(const Node & node, date d)
  {
    VAR(lastSeagulGroup)->setSpline(modeLinear);
    vector3f pos = VAR(lastSeagulGroup)->get3((float) d);
    glTranslatef(pos.x, pos.y, pos.z);
  }
}
