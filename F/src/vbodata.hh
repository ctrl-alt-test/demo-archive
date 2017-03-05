//
// Données pour un VBO
//

#ifndef		VBO_DATA_HH
# define	VBO_DATA_HH

#include "array.hh"
#include "mesh.hh"
#include "vector.hh"

namespace VBO
{
  class Element
  {
  public:
    Element();
    ~Element();

    unsigned int id() const;
    void use() const;

    static void unUse();

    void setupData(int size, void * data) const;
    void setupData(const Array <vertex> & chunk
		   DBGARG(char * description)) const;

  private:
    unsigned int _id;
  };
}


#include "vbodata.hxx"

#endif		// VBO_DATA_HH
