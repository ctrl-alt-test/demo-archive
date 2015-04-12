//
// Données pour un VBO
//

#ifndef		VBO_DATA_HH
# define	VBO_DATA_HH

#include "vector.hh"
#include "array.hh"

namespace VBO
{
  typedef struct // FIXME : aurait plus sa place avec les VBOs
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
  } vertex;

  class Element
  {
  public:
    Element();
    ~Element();

    unsigned int id() const;
    void use(int attribData) const;

    static void unUse();

    void setupData(unsigned int size, void * data, int attribIndex) const;
    void setupData(const Array <vertex> & chunk, int attribData) const;

  private:
    void _setBuffers(int attribIndex) const;

    unsigned int _id;
  };
}


#include "vbodata.hxx"

#endif		// VBO_DATA_HH
