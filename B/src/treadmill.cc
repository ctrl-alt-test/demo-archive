//
// Tapis roulant
//

#include "treadmill.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "anim.hh"
#include "basicTypes.hh"
#include "cube.hh"
#include "vbobuild.hh"

#define CHUNKED_TREADMILL 1
#define CHUNK_LENGTH 64 // Doit être un multiple de 4 (pour les anims)
#define CHUNK_WIDTH 7

namespace Treadmill
{
  unsigned int numberOfVerticesChunk0 = 0;
  unsigned int numberOfVerticesChunk1 = 0;
  unsigned int numberOfVerticesChunk2 = 0;
  unsigned int numberOfVerticesChunk3 = 0;
  VBO::vertex * chunk = NULL;
  VBO::vertex * chunk0 = NULL; // FIXME : serait encore plus performant avec un seul chunk
  VBO::vertex * chunk1 = NULL;
  VBO::vertex * chunk2 = NULL;
  VBO::vertex * chunk3 = NULL;

  unsigned int chunkVerticesCount(unsigned int part)
  {
    unsigned int numberOfCubes = 0;

    const unsigned int halfWidth = CHUNK_WIDTH / 2;
    for (unsigned int j = 0; j < CHUNK_LENGTH; ++j)
      for (unsigned int i = 0; i < CHUNK_WIDTH; ++i)
      {
	const int shift = (i >= halfWidth ? i : CHUNK_WIDTH - i - 1) + j;
	if (shift % 4 == part)
	{
	  ++numberOfCubes;
	}
      }
    return numberOfCubes * Cube::numberOfVertices;
  }

  void generateChunkPart(unsigned int part, VBO::vertex * dest)
  {
    unsigned int count = 0;
    const unsigned int halfWidth = CHUNK_WIDTH / 2;
    for (unsigned int j = 0; j < CHUNK_LENGTH; ++j)
      for (unsigned int i = 0; i < CHUNK_WIDTH; ++i)
      {
	const int shift = (i >= halfWidth ? i : CHUNK_WIDTH - i - 1) + j;
	if (shift % 4 == part)
	{
	  const vector3f p = {1.5f * j, 0, 1.f * i};
	  const float c = 0.3f + 0.7f * msys_frand();
	  const vector3f color = {c, c, c};
	  VBO::addCubeToChunk(dest + count * Cube::numberOfVertices, p, color);
	  ++count;
	}
      }
  }

  void generateMeshes()
  {
    numberOfVerticesChunk0 = chunkVerticesCount(0);
    numberOfVerticesChunk1 = chunkVerticesCount(1);
    numberOfVerticesChunk2 = chunkVerticesCount(2);
    numberOfVerticesChunk3 = chunkVerticesCount(3);
    const unsigned int numberOfVertices = (numberOfVerticesChunk0 +
					   numberOfVerticesChunk1 +
					   numberOfVerticesChunk2 +
					   numberOfVerticesChunk3 +
					   0);
    chunk = (VBO::vertex *)msys_mallocAlloc(numberOfVertices * sizeof(VBO::vertex));
    DBG("Chunk treadmill : allocation de %u sommets, %u octets", numberOfVertices, numberOfVertices * sizeof(VBO::vertex));

    chunk0 = chunk;
    chunk1 = &(chunk0[numberOfVerticesChunk0]);
    chunk2 = &(chunk1[numberOfVerticesChunk1]);
    chunk3 = &(chunk2[numberOfVerticesChunk2]);

//     unsigned int offset = 0;
//     generateChunkPart(0, offset); offset += numberOfVerticesChunk0;
//     generateChunkPart(1, offset); offset += numberOfVerticesChunk1;
//     generateChunkPart(2, offset); offset += numberOfVerticesChunk2;
//     generateChunkPart(3, offset);

    generateChunkPart(0, chunk0);
    generateChunkPart(1, chunk1);
    generateChunkPart(2, chunk2);
    generateChunkPart(3, chunk3);
  }

  void addToList(RenderList & renderList,
		 date birthDate, date deathDate, date animStopDate,
		 unsigned int length, unsigned int width,
		 Shader::id shaderId)
  {
    glPushMatrix();
    glTranslatef(0, -0.2f, width * -0.1f + 0.1f);

    const unsigned int halfWidth = width / 2;
    for (unsigned int j = 0; j < length; ++j)
      for (unsigned int i = 0; i < width; ++i)
      {

#if CHUNKED_TREADMILL

	if (0 == i % CHUNK_WIDTH && 0 == j % CHUNK_LENGTH)
	{
	  glPushMatrix();
	  glScalef(0.2f, 0.2f, 0.2f);
	  glTranslatef(1.5f * j, 0, 1.f * i);

	  Renderable chunk0(deathDate, shaderId, birthDate,
			    1.f, numberOfVerticesChunk0, VBO::treadmillChunk0);
	  chunk0.setAnimation(Anim::slowTreadmill);
	  chunk0.setAnimationStepShift(0.75f);
	  chunk0.setAnimationStopDate(animStopDate);
	  renderList.add(chunk0);

	  Renderable chunk1(deathDate, shaderId, birthDate,
			    1.f, numberOfVerticesChunk1, VBO::treadmillChunk1);
	  chunk1.setAnimation(Anim::slowTreadmill);
	  chunk1.setAnimationStepShift(0.5f);
	  chunk1.setAnimationStopDate(animStopDate);
	  renderList.add(chunk1);

	  Renderable chunk2(deathDate, shaderId, birthDate,
			    1.f, numberOfVerticesChunk2, VBO::treadmillChunk2);
	  chunk2.setAnimation(Anim::slowTreadmill);
	  chunk2.setAnimationStepShift(0.25f);
	  chunk2.setAnimationStopDate(animStopDate);
	  renderList.add(chunk2);

	  Renderable chunk3(deathDate, shaderId, birthDate,
			    1.f, numberOfVerticesChunk3, VBO::treadmillChunk3);
	  chunk3.setAnimation(Anim::slowTreadmill);
	  chunk3.setAnimationStopDate(animStopDate);
	  renderList.add(chunk3);

	  glPopMatrix();
	}

#else

	const int shift = (i >= halfWidth ? i : width - i - 1) + j;
	const float animStepShift = (shift % 4) * -0.25f;
	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(1.5f * j, 0, 1.f * i);
	glRotatef(-90.f, 1.f, 0, 0);
	renderList.add(Renderable(deathDate, shaderId, Anim::squareCycle, animStepShift, 0, birthDate));
	glPopMatrix();

#endif // CHUNKED_TREADMILL

      }
    glPopMatrix();
  }
}
