#include "space.hh"

#include "starfield.hh"
#include "timing.hh"

#define START_DATE	starfieldStartDate
#define END_DATE	starfieldEndDate

namespace Space
{

  Conway::Starfield * starfield;

  void createCube(RenderList& renderList);

  void create(RenderList& renderList)
  {
    starfield = new Conway::Starfield(150, 150);
    starfield->createCubes();
    createCube(renderList);
  }

  void createCube(RenderList& renderList)
  {
    glPushMatrix();
    glLoadIdentity();
    Renderable cube(END_DATE, Shader::color, START_DATE,
		    1.f, Cube::numberOfVertices, VBO::theCube);
    cube.setAnimation(Anim::conwayCube);
    renderList.add(cube);
    glPopMatrix();
  }

  void update(RenderList& renderList, date updateDate)
  {
    if (starfield && START_DATE <= updateDate && updateDate <= END_DATE)
    {
      starfield->update(renderList, updateDate);
    }
  }
}
