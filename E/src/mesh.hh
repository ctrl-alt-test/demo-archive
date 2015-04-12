//
// Définition des types pour les meshs
//

#ifndef		MESH_HH
# define	MESH_HH

#if DEBUG
#include "array.hh"
#endif // DEBUG


typedef struct
{
  float x; // Position du sommet
  float y;
  float z;

  float nx; // Normale au sommet
  float ny;
  float nz;

  float tx; // Tangente au sommet
  float ty;
  float tz;

  float r; // Couleur du sommet // FIXME : tester avec un unsigned int RGBA
  float g;
  float b;

  float u; // Coordonnée de texture du sommet
  float v;
  float w;
  float q;
} vertex;

typedef struct
{
  float x1;
  float y1;
  float x2;
  float y2;
  float x3;
  float y3;
  float x4;
  float y4;
} quad;

#if DEBUG

void immediateArrayDraw(const Array<vertex> & vertices, int vTanIndex);

#endif // DEBUG

vertex transformedVertex(const float m[16], const vertex & v);

#endif		// MESH_HH
