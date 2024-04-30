//
// Tous les VBOs utilisés
//

#include "vbos.hh"

#include "demo.hh"
#include "intro.hh"
#include "mesh/cube.hh"
#include "mesh/mesh.hh"
#include "shaders.hh"
#include "vbodata.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <stdio.h>

namespace VBO
{
  Element * list = NULL;
  int elementSize[256]; // TODO: déplacer ce tableau dans DescVBO.

  struct DescVBO
  {
    // int elementSize;
    const char* name;
  };
  DescVBO elements[256];

#if DEBUG
  int numberOfVBOs = 0;

  int getVBO(const char *name)
  {
    for (int i = 0; i < numberOfVBOs; i++)
    {
      assert(elements[i].name != NULL);
      assert(name != NULL);
      if (strcmp(name, elements[i].name) == 0)
        return i;
    }
    return -1;
  }

  id newVBO(const char *name)
  {
    int vbo = getVBO(name);
    if (vbo >= 0) return (VBO::id) vbo;

    DescVBO desc = {name};
    elements[numberOfVBOs] = desc;
    return (VBO::id) (numberOfVBOs++);
  }

  id newVBOs(const char* name, int nb)
  {
    int vbo = getVBO(name);
    if (vbo >= 0) return (VBO::id) vbo;

    DescVBO desc = {name};
    int old = numberOfVBOs;
    numberOfVBOs += nb;
    for (int i = old; i < numberOfVBOs; i++)
      elements[i] = desc;
    return (VBO::id) (old);
  }

  id forceGetVBO(const char *name)
  {
    int res = getVBO(name);
    assert(res >= 0);
    assert(elements[res].name != NULL);
    assert(elementSize[res] > 0);
    return (id) res;
  }

  void exportIds()
  {
    FILE * fp = fopen("data/exported-vboid.hh", "w");
    const char *macroName = "EXPORTED_VBOID_HH";
    assert(fp != NULL);
    fprintf(fp, "// Generated file\n\n#ifndef %s\n#define %s\n\n", macroName, macroName);

    fprintf(fp, "enum id : char {\n");
    int i;
    for (i = 0; i < numberOfVBOs; i++)
    {
      if (i == 0 || strcmp(elements[i].name, elements[i-1].name) != 0)
        fprintf(fp, "    %s = %d,\n", elements[i].name, i);
    }
    fprintf(fp, "\n    numberOfVBOs = %d,\n", i);
    fprintf(fp, "};\n\n#endif\n");
    fclose(fp);
  }

#endif

  void setupData(id vboId, const Mesh::MeshStruct & mesh
		 DBGARG(char * description))
  {
    setupData(vboId, mesh.vertices DBGARG(description));
  }

  void setupData(id vboId, const Array <vertex> & chunk
		 DBGARG(char * description))
  {
    // When reloading, we can overwrite the existing elements.
    IFDBG(if (!intro.initDone))
      assert(elementSize[vboId] == 0);

    assert(chunk.size > 0);
    VBO::list[vboId].setupData(chunk DBGARG(description));
    elementSize[vboId] = chunk.size;
  }

  void loadVBOs()
  {
    START_TIME_EVAL;

    const unsigned int numberOfSteps = 1;

    list = new Element[256];

    DBG("Generation of a cube");
    VBO::id cubeId = NEW_VBO(singleCube);
    list[cubeId].setupData(Cube::numberOfVertices * sizeof(vertex), Cube::vertices);
    elementSize[cubeId] = Cube::numberOfVertices;

    DBG("Mesh generation");
    Demo::generateMeshes();

    Loading::update();
    OGL_ERROR_CHECK("End of loadVBOs");

    END_TIME_EVAL("VBO setup");
  }

  // FIXME : destruction des VBO
}
