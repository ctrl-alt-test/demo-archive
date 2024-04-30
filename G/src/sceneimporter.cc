
#include "sceneimporter.hh"

#include "renderable.hh"
#include "vbodata.hh"
#include "vbos.hh"

// Données générées par l'exporteur
#include "testimportedscene.cc"

#include "sys/msys_debug.h"


void importNode(Array<Node> * nodeList,
		const nodeDescription & desc)
{
  const vector3f p(desc.position[0],
		   desc.position[1],
		   desc.position[2]);
  quaternion q(desc.quaternion[0],
	       desc.quaternion[1],
	       desc.quaternion[2],
	       desc.quaternion[3]);
  normalize(q);

  setToModelView(computeMatrix(Transformation(q, p)));

  Node node(0, -1);

//   Node node(desc.birthDate, desc.deathDate);
//   node.setAnimation(desc.animId);
//   node.setAnimationStepShift(desc.animStepShift);
//   node.setAnimationStartDate(desc.animStartDate);
//   node.setAnimationStopDate(desc.animStopDate);

  nodeList->add(node);
}

void importNodes(Array<Node> * nodeList)
{
  for (unsigned int i = 0; i < numberOfNodes; ++i)
  {
    importNode(nodeList, nodeDescriptions[i]);
  }
}

void addVerticesToMesh(Array<vertex> & mesh,
		       const meshDescription & desc,
		       unsigned int verticesStart,
		       unsigned int indicesStart,
		       bool invertX, bool invertY, bool invertZ)
{
  const bool invertOrder = invertX ^ invertY ^ invertZ;

  const float xScale = (invertX ? -desc.scale[0] : desc.scale[0]);
  const float yScale = (invertY ? -desc.scale[1] : desc.scale[1]);
  const float zScale = (invertZ ? -desc.scale[2] : desc.scale[2]);

  const unsigned int numberOfFaces = desc.numberOfIndices / 3;
  for (unsigned int face = 0; face < numberOfFaces; ++face)
  {
    vertex v[3];

    const unsigned int iIndex = indicesStart + 3 * face;
    for (unsigned int i = 0; i < 3; ++i)
    {
      const unsigned int vIndex = verticesStart + 3 * indices[iIndex + i];
      unsigned int j = i;
      if (invertOrder && i == 1) j = 2;
      if (invertOrder && i == 2) j = 1;
      v[j].p = vector3f(xScale * float(vertices[vIndex]    ),
			yScale * float(vertices[vIndex + 1]),
			zScale * float(vertices[vIndex + 2]));
    }

    vector3f normal = (v[1].p - v[0].p) ^ (v[2].p - v[0].p);
    if (norm(normal) != 0)
    {
      normalize(normal);
      v[0].n = v[1].n = v[2].n = normal;

      vector3f tangent = (v[1].p - v[0].p);
      normalize(tangent);
      v[0].t = v[1].t = v[2].t = tangent;
    }
    else
    {
      // FIXME : se débrouiller à l'export pour que ça n'arrive pas :-(
      v[0].n = v[1].n = v[2].n = vector3f(0, 1.f, 0);
      v[0].t = v[1].t = v[2].t = vector3f(1.f, 0, 0);
    }

//     v[0].r = 1.f;
//     v[0].g = 1.f;
//     v[0].b = 1.f;
//     v[1].r = 1.f;
//     v[1].g = 1.f;
//     v[1].b = 1.f;
//     v[2].r = 1.f;
//     v[2].g = 1.f;
//     v[2].b = 1.f;
    // FIXME : coordonnées de texture, couleur

    mesh.add(v[0]);
    mesh.add(v[1]);
    mesh.add(v[2]);
  }
}

void addVerticesToMeshWithSymmetry2(Array<vertex> & mesh,
				    const meshDescription & desc,
				    unsigned int verticesStart,
				    unsigned int indicesStart,
				    unsigned char symmetry)
{
  addVerticesToMesh(mesh, desc, verticesStart, indicesStart,
		    (symmetry & X_SYMMETRY) != 0,
		    (symmetry & Y_SYMMETRY) != 0,
		    (symmetry & Z_SYMMETRY) != 0);
  if ((symmetry & Z_SYMMETRY) != 0)
  {
    addVerticesToMesh(mesh, desc, verticesStart, indicesStart,
		      (symmetry & X_SYMMETRY) != 0,
		      (symmetry & Y_SYMMETRY) != 0,
		      false);
  }
}

void addVerticesToMeshWithSymmetry1(Array<vertex> & mesh,
				    const meshDescription & desc,
				    unsigned int verticesStart,
				    unsigned int indicesStart,
				    unsigned char symmetry)
{
  addVerticesToMeshWithSymmetry2(mesh, desc, verticesStart, indicesStart,
				 symmetry);
  if ((symmetry & Y_SYMMETRY) != 0)
  {
    addVerticesToMeshWithSymmetry2(mesh, desc, verticesStart, indicesStart,
				   (symmetry & (X_SYMMETRY | Z_SYMMETRY)));
  }
}

void addVerticesToMeshWithSymmetry(Array<vertex> & mesh,
				   const meshDescription & desc,
				   unsigned int verticesStart,
				   unsigned int indicesStart,
				   unsigned char symmetry)
{
  addVerticesToMeshWithSymmetry1(mesh, desc, verticesStart, indicesStart,
				 symmetry);
  if ((symmetry & X_SYMMETRY) != 0)
  {
    addVerticesToMeshWithSymmetry1(mesh, desc, verticesStart, indicesStart,
				   symmetry & (Y_SYMMETRY | Z_SYMMETRY));
  }
}

void importMesh(Array<Renderable> * renderableList,
		const meshDescription & desc,
		unsigned int verticesStart,
		unsigned int indicesStart)
{
  const unsigned int numberOfVertices = ((desc.symmetry & X_SYMMETRY ? 2 : 1) *
					 (desc.symmetry & Y_SYMMETRY ? 2 : 1) *
					 (desc.symmetry & Z_SYMMETRY ? 2 : 1) *
					 desc.numberOfIndices);
  Array<vertex> mesh(numberOfVertices);
  addVerticesToMeshWithSymmetry(mesh, desc, verticesStart, indicesStart,
				desc.symmetry);
  VBO::setupData(desc.vboId, mesh DBGARG("imported mesh"));

  Renderable renderable(Material::none, desc.vboId);
  renderable.setPrimitiveType(GL_TRIANGLES);
//   renderable.setId(desc.id);
// #if DEBUG
//   renderable.setTextures(desc.textureId,
// 			 Texture::testBump,
// 			 Texture::testSpecular);
// #endif
//   renderable.setTexture2(desc.texture2);
  renderableList->add(renderable);
}

void importMeshes(Array<Renderable> * renderableList)
{
  unsigned int verticesStart = 0;
  unsigned int indicesStart = 0;
  for (unsigned int i = 0; i < numberOfMeshes; ++i)
  {
    importMesh(renderableList, meshDescriptions[i],
	       verticesStart, indicesStart);
    verticesStart += 3 * meshDescriptions[i].numberOfVertices;
    indicesStart += meshDescriptions[i].numberOfIndices;
  }
}

//
// Relier les noeuds de l'arbre et accrocher les meshes aux noeuds
//
void resolveDependencies(Array<Node> * nodeList,
			 Array<Renderable> * renderableList)
{
  unsigned int childIt = 0;
  unsigned int renderableIt = 0;

  assert(numberOfNodes == nodeList->size);
  assert(numberOfMeshes == renderableList->size);
  for (unsigned int i = 0; i < numberOfNodes; ++i)
  {
    DBG("Attaching things to node %d...", i);

    const nodeDescription & desc = nodeDescriptions[i];
    Node * node = nodeList->elt + i;


    DBG("  Attaching %d nodes...", desc.numberOfChildren);
    for (unsigned int j = 0; j < desc.numberOfChildren; ++j)
    {
      assert(childIt < (sizeof(nodeChildren) / sizeof(nodeChildren[0])));
      const int childId = nodeChildren[childIt++];

      assert(nodeList->size > childId);
      Node * child = nodeList->elt + childId;

      DBG("    Attaching node %d to node %d", childId, i);
      child->attachToNode(node);
    }
//     assert(node->children().size == desc.numberOfChildren);
    assert(childIt <= (sizeof(nodeChildren) / sizeof(nodeChildren[0])));


    DBG("  Attaching %d meshes...", desc.numberOfRenderables);
    for (unsigned int j = 0; j < desc.numberOfRenderables; ++j)
    {
      assert(renderableIt < (sizeof(renderableIds) / sizeof(renderableIds[0])));
      const int renderableId = (int)renderableIds[renderableIt++];
      assert(renderableList->size > renderableId);
      Renderable & renderable = (*renderableList)[renderableId];

      DBG("    Attaching mesh %d to node %d", renderableId, i);
      node->attachRenderable(renderable);
    }
//     assert(node->visiblePart().size == desc.numberOfRenderables);
    assert(renderableIt <= (sizeof(renderableIds) / sizeof(renderableIds[0])));
  }
  DBG("Done resolving dependencies\n");
}

//
// Prototype temporaire en attendant de trouver mieux
//
Node * importScene()
{
  DBG("Importing scene...");

  /*
  Array<Node> * foobar = new Array<Node>(1);
  Node foo(0, -1);
  Renderable bar;
  foo.attachRenderable(bar);
  foobar->add(foo);
  */

  // FIXME : et on en fait quoi de ces tableaux ?
  Array<Node> * nodeList = new Array<Node>(numberOfNodes);
  Array<Renderable> * renderableList = new Array<Renderable>(numberOfMeshes);

  importNodes(nodeList);
  importMeshes(renderableList);
  resolveDependencies(nodeList, renderableList);

// #if DEBUG
//   assert(nodeList->size == nodeList->max_size);
//   for (int i = 1; i < nodeList->size; ++i)
//     assert((*nodeList)[i].parent() != NULL);
// #endif

  DBG("Import done");

  Node * root = &((*nodeList)[0]);
//   Node * root = &((*foobar)[0]);

  return root;
}
