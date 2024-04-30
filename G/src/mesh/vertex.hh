//
// Définition d'un sommet
//

#ifndef		VERTEX_HH
# define	VERTEX_HH

#include "algebra/vector3.hh"

#define VERTEX_ATTR_POSITION   1
#define VERTEX_ATTR_NORMAL     1
#define VERTEX_ATTR_TANGENT    1
#define VERTEX_ATTR_COLOR      1
#define VERTEX_ATTR_TEX_COORD  1
#define VERTEX_ATTR_BARY_COORD 1
#define VERTEX_ATTR_ID         1

struct vertex
{
  //
  // Cette liste est dupliquée dans :
  // - shaderProgram.cc (Program::bindAttributes)
  //
  enum attribute
    {
#if VERTEX_ATTR_TANGENT
      tangent,          // Tangente
#endif
#if VERTEX_ATTR_COLOR
      color,            // Couleur
#endif
#if VERTEX_ATTR_BARY_COORD
      barycentricCoord, // Coordonnées barycentriques
#endif
#if VERTEX_ATTR_ID
      identifier,       // Identifiant
#endif
      numberOfAttributes
    };

#if VERTEX_ATTR_POSITION
  vector3f p; // Position du sommet
#endif

#if VERTEX_ATTR_NORMAL
  vector3f n; // Normale au sommet
#endif

#if VERTEX_ATTR_TANGENT
  vector3f t; // Tangente au sommet
#endif

#if VERTEX_ATTR_COLOR
  float r; // Couleur du sommet
  float g;
  float b;
#endif

#if VERTEX_ATTR_TEX_COORD
  float u; // Coordonnée de texture du sommet
  float v; // Usuellement (s, t, r, q), mais r et t sont déjà pris
  float w;
  float q;
#endif

#if VERTEX_ATTR_BARY_COORD
  float d0; // Coordonnées barycentriques
  float d1;
  float d2;
  float d3;
#endif

#if VERTEX_ATTR_ID
  float id; // Identifiant du sommet
#endif

#if DEBUG
  void immediateDraw() const;
#endif

  static int attributeLocation(attribute attribute);
  static void setAttributePointers(const void * pointer);
  static void activateRenderStates();
  static void deactivateRenderStates();
};

#endif		// VERTEX_HH
