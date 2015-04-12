//
// Tous les VBOs utilisés
//

#include "vbos.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "factory.hh"

#include "cube.hh"
#include "cylinder.hh"
#include "shaders.hh"
#include "trails.hh"
#include "vbobuild.hh"
#include "vbodata.hh"
#include "vboid.hh"

namespace VBO
{
  Element * list = NULL;

  void loadVBOs(Loading::ProgressDelegate * pd, int low, int high)
  {
    START_TIME_EVAL;

    const unsigned int numberOfSteps = 1;
    const int ldStep = (high - low) / numberOfSteps;
    int currentLd = low;

    list = new Element[numberOfVBOs];
//     int index = oglGetAttribLocation(Shader::list[Shader::default].id(), "vTan");
//     OGL_ERROR_CHECK("VBO::use, glGetAttribLocation()");

    // Cube de base
    // ============
    DBG("Génération du mesh Cube");
    list[singleCube].setupData(Cube::numberOfVertices * sizeof(vertex), Cube::vertices/*, index*/);

#if (!DEBUG_TRAILS)

    // Terrain
    // =======
    DBG("Génération des meshs du terrain");
    Factory::generateMeshes();
    list[floor].setupData(Factory::floorChunk);
    list[buildings].setupData(Factory::buildingsChunk);

#endif // DEBUG_TRAILS

    // Formes de révolution
    // ====================
    /*
    DBG("Génération des meshs Mesh");
    Mesh::generateTestMeshes();
    */

    pd->func(pd->obj, high);
    OGL_ERROR_CHECK("Fin du loadVBOs");

    END_TIME_EVAL("VBO setup");
  }

  // FIXME : destruction des VBO
}
