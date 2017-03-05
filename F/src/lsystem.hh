//
// L system
//

#ifndef		L_SYSTEM_HH
# define	L_SYSTEM_HH

#include "basicTypes.hh"
#include "renderlist.hh"

namespace LSystem
{
  typedef void(*Function)(date birth, date death, unsigned int depth);

  class System
  {
  public:
    System();

    void setRules(const char * X, const char * F,
		  const char * G, const char * H);

    // Première partie : développement de la grammaire
    void develop(int iterations);

    // Seconde partie : interprétation de l'état
    Array<Node *> * generateNodes(float angle = 30.f,
				  float reduction = 1.f,
				  float forwardLength = 2.5f,
				  unsigned int nodesPerForward = 1,
				  date birthDate = 0,
				  date deathDate = -1,
				  int birthDateIncrement = 0,
				  int deathDateIncrement = 0);

  private:
    void _iterate(Array<char> & newState, Array<char> & oldState);

  private:
    System & operator = (const System &);

    Array<char> _X;
    Array<char> _F;
    Array<char> _G;
    Array<char> _H;

    Array<char> _developped;
  };
}

#endif		// L_SYSTEM_HH
