
#include "anims.hh"
#include "algebra/quaternion.hh"
#include "algebra/vector3.hh"

#include "node.hh"
#include "shaderid.hh"
#include "textureid.hh"

struct nodeDescription
{
//   date birthDate;
//   date deathDate;
//   Anim::id animId;
//   date animStepShift;
//   date animStartDate;
//   date animStopDate;

  float position[3];
  char quaternion[4];
  unsigned char numberOfChildren;
  unsigned char numberOfRenderables;
};

#define NO_SYMMETRY 0
#define X_SYMMETRY  1
#define Y_SYMMETRY  2
#define Z_SYMMETRY  4

struct meshDescription
{
  float scale[3];
  unsigned short numberOfVertices;
  unsigned short numberOfIndices;
  unsigned char symmetry;

  VBO::id vboId;
  Shader::id shaderId;
  Texture::id textureId;
//   Texture::id texture2Id;
//   Texture::id bumpTextureId;
//   Texture::id specularTextureId;
};

Node * importScene();
