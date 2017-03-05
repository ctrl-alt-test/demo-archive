//
// Tous les VBOs utilisés
//

#include "vbos.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "workshop.hh"

#include "cube.hh"
#include "revolution.hh"
#include "shaders.hh"
#include "vbodata.hh"
#include "vboid.hh"

namespace VBO
{
  Element * list = NULL;
  int elementSize[numberOfVBOs];

  void setupData(id id, const Array <vertex> & chunk
		 DBGARG(char * description))
  {
    assert(elementSize[id] == 0);
    assert(chunk.size > 0);
    VBO::list[id].setupData(chunk DBGARG(description));
    elementSize[id] = chunk.size;
  }

  void generatePave(VBO::id id, float x, float y, float z)
  {
    Array<vertex> pave_data(Cube::numberOfVertices);
    Mesh::generatePave(pave_data, x, y, z);
    setupData(id, pave_data DBGARG("pave"));
  }

  void loadVBOs()
  {
    START_TIME_EVAL;

    const unsigned int numberOfSteps = 1;

    list = new Element[numberOfVBOs];
//     int index = oglGetAttribLocation(Shader::list[Shader::default].id(), "vTan");
//     OGL_ERROR_CHECK("VBO::use, glGetAttribLocation()");

    DBG("Génération du mesh Cube");
    list[singleCube].setupData(Cube::numberOfVertices * sizeof(vertex), Cube::vertices);
    elementSize[singleCube] = Cube::numberOfVertices;

    DBG("Génération des autres meshs");
    Workshop::generateMeshes();

    Loading::update();
    OGL_ERROR_CHECK("Fin du loadVBOs");

    END_TIME_EVAL("VBO setup");
  }

  // FIXME : destruction des VBO
}
