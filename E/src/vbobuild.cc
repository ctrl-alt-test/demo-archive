//
// Construction de VBO
//

#include "vbobuild.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"

#include "cube.hh"

namespace VBO
{
  // =========================================================================
  // Versions pour les hommes, où il faut gérer les indices

  // Version de base, avec position et taille
  unsigned int addCubeToChunk(vertex * dest, const vector3f &p,
			      float size,
			      bool x1face, bool x2face,
			      bool y1face, bool y2face,
			      bool z1face, bool z2face)
  {
    assert(x1face || x2face || y1face || y2face || z1face || z2face);
    unsigned int count = 0;
    for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      if ((x1face && i < 4) ||
	  (x2face && (i >= 4 && i < 8)) ||
	  (y1face && (i >= 8 && i < 12)) ||
	  (y2face && (i >= 12 && i < 16)) ||
	  (z1face && (i >= 16 && i < 20)) ||
	  (z2face && i >= 20))
      {
	dest[count] = Cube::vertices[i];
	vertex & dst = dest[count];
	dst.x = dst.x * size + p.x;
	dst.y = dst.y * size + p.y;
	dst.z = dst.z * size + p.z;
	++count;
      }
    return count;
  }

  // Version de base, avec position et couleur
  unsigned int addCubeToChunk(vertex * dest, const vector3f &p,
			      const vector3f &color,
			      bool x1face, bool x2face,
			      bool y1face, bool y2face,
			      bool z1face, bool z2face)
  {
    assert(x1face || x2face || y1face || y2face || z1face || z2face);
    unsigned int count = 0;
    for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      if ((x1face && i < 4) ||
	  (x2face && (i >= 4 && i < 8)) ||
	  (y1face && (i >= 8 && i < 12)) ||
	  (y2face && (i >= 12 && i < 16)) ||
	  (z1face && (i >= 16 && i < 20)) ||
	  (z2face && i >= 20))
      {
	dest[count] = Cube::vertices[i];
	vertex & dst = dest[count];
	dst.x += p.x;
	dst.y += p.y;
	dst.z += p.z;
	dst.r = color.x;
	dst.g = color.y;
	dst.b = color.z;
	++count;
      }
    return count;
  }

  // Version de base, avec position et couleur par face
  unsigned int addCubeToChunk(vertex * dest, const vector3f &p,
			      const vector3f & x1Color,
			      const vector3f & x2Color,
			      const vector3f & y1Color,
			      const vector3f & y2Color,
			      const vector3f & z1Color,
			      const vector3f & z2Color,
			      bool x1face, bool x2face,
			      bool y1face, bool y2face,
			      bool z1face, bool z2face)
  {
    assert(x1face || x2face || y1face || y2face || z1face || z2face);
    unsigned int count = 0;
    for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      if ((x1face && i < 4) ||
	  (x2face && (i >= 4 && i < 8)) ||
	  (y1face && (i >= 8 && i < 12)) ||
	  (y2face && (i >= 12 && i < 16)) ||
	  (z1face && (i >= 16 && i < 20)) ||
	  (z2face && i >= 20))
      {
	dest[count] = Cube::vertices[i];
	vertex & dst = dest[count];
	dst.x += p.x;
	dst.y += p.y;
	dst.z += p.z;
	if (i < 4)
	{
	  dst.r = x1Color.x;
	  dst.g = x1Color.y;
	  dst.b = x1Color.z;
	}
	else if (i < 8)
	{
	  dst.r = x2Color.x;
	  dst.g = x2Color.y;
	  dst.b = x2Color.z;
	}
	else if (i < 12)
	{
	  dst.r = y1Color.x;
	  dst.g = y1Color.y;
	  dst.b = y1Color.z;
	}
	else if (i < 16)
	{
	  dst.r = y2Color.x;
	  dst.g = y2Color.y;
	  dst.b = y2Color.z;
	}
	else if (i < 20)
	{
	  dst.r = z1Color.x;
	  dst.g = z1Color.y;
	  dst.b = z1Color.z;
	}
	else
	{
	  dst.r = z2Color.x;
	  dst.g = z2Color.y;
	  dst.b = z2Color.z;
	}

	++count;
      }
    return count;
  }

  // Version avec matrice
  unsigned int addCubeToChunk(vertex * dest, const float m[16],
			      const vector3f &color,
			      bool x1face, bool x2face,
			      bool y1face, bool y2face,
			      bool z1face, bool z2face)
  {
    assert(x1face || x2face || y1face || y2face || z1face || z2face);
    unsigned int count = 0;
    for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      if ((x1face && i < 4) ||
	  (x2face && (i >= 4 && i < 8)) ||
	  (y1face && (i >= 8 && i < 12)) ||
	  (y2face && (i >= 12 && i < 16)) ||
	  (z1face && (i >= 16 && i < 20)) ||
	  (z2face && i >= 20))
      {
	dest[count] = Cube::vertices[i];
	vertex & dst = dest[count];
	vector3f p = {dst.x, dst.y, dst.z};
	vector3f n = {dst.nx, dst.ny, dst.nz};
	vector3f t = {dst.tx, dst.ty, dst.tz};
	computeMatrixPointProduct(m, p);
	computeMatrixVectorProduct(m, n);
	computeMatrixVectorProduct(m, t);
	dst.x = p.x;
	dst.y = p.y;
	dst.z = p.z;
	dst.nx = n.x;
	dst.ny = n.y;
	dst.nz = n.z;
	dst.tx = t.x;
	dst.ty = t.y;
	dst.tz = t.z;
	dst.r = color.x;
	dst.g = color.y;
	dst.b = color.z;

	++count;
      }
    return count;
  }


  // =========================================================================
  // Versions équivalentes, à base de tableau

  void addCube(Array<vertex> &vbo, const vector3f & pos, float size,
	       bool x1face, bool x2face,
	       bool y1face, bool y2face,
	       bool z1face, bool z2face)
  {
    // FIXME : l'assert n'est plus bonne depuis qu'on peut retirer des faces
    assert(vbo.max_size >= vbo.size + (int)Cube::numberOfVertices);
    vertex *ptr = vbo.elt + vbo.size;
    vbo.size += VBO::addCubeToChunk(ptr, pos, size,
				    x1face, x2face,
				    y1face, y2face,
				    z1face, z2face);
  }

  void addCube(Array<vertex> &vbo, const vector3f & pos,
	       const vector3f & color,
	       bool x1face, bool x2face,
	       bool y1face, bool y2face,
	       bool z1face, bool z2face)
  {
    // FIXME : l'assert n'est plus bonne depuis qu'on peut retirer des faces
    assert(vbo.max_size >= vbo.size + (int)Cube::numberOfVertices);
    vertex *ptr = vbo.elt + vbo.size;
    vbo.size += VBO::addCubeToChunk(ptr, pos, color,
				    x1face, x2face,
				    y1face, y2face,
				    z1face, z2face);
  }

  void addCube(Array<vertex> &vbo, const vector3f & pos,
	       const vector3f & x1Color,
	       const vector3f & x2Color,
	       const vector3f & y1Color,
	       const vector3f & y2Color,
	       const vector3f & z1Color,
	       const vector3f & z2Color,
	       bool x1face, bool x2face,
	       bool y1face, bool y2face,
	       bool z1face, bool z2face)
  {
    // FIXME : l'assert n'est plus bonne depuis qu'on peut retirer des faces
    assert(vbo.max_size >= vbo.size + (int)Cube::numberOfVertices);
    vertex *ptr = vbo.elt + vbo.size;
    vbo.size += VBO::addCubeToChunk(ptr, pos,
				    x1Color, x2Color,
				    y1Color, y2Color,
				    z1Color, z2Color,
				    x1face, x2face,
				    y1face, y2face,
				    z1face, z2face);
  }

  void addCube(Array<vertex> & vbo, const float m[16],
	       const vector3f & color,
	       bool x1face, bool x2face,
	       bool y1face, bool y2face,
	       bool z1face, bool z2face)
  {
    assert(vbo.max_size >= vbo.size + (int)Cube::numberOfVertices);
    vertex *ptr = vbo.elt + vbo.size;
    vbo.size += VBO::addCubeToChunk(ptr, m, color,
				    x1face, x2face,
				    y1face, y2face,
				    z1face, z2face);
  }


  // =========================================================================
  // Version last minute VBO service

  // Si on veut reutiliser le code, attention a la couleur aleatoire. :)
  void addFromRenderList(Array<vertex> & vbo,
			 const RenderList & renderList,
			 bool x1face, bool x2face,
			 bool y1face, bool y2face,
			 bool z1face, bool z2face)
  {
    for (int i = 0; i < renderList.size; i++)
    {
      const Renderable & element = renderList[i];
      const vector3f color = {msys_frand(), 1., 1.};

      addCube(vbo, element.viewMatrix(), color,
	      x1face, x2face,
	      y1face, y2face,
	      z1face, z2face);
    }
  }
}
