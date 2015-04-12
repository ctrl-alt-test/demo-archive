//
// Moteur de l'usine
//

#ifndef		ROTOR_HH
# define	ROTOR_HH

#include "basicTypes.hh"
#include "renderlist.hh"
#include "shaderid.hh"

namespace Rotor
{
  extern const date pistonCycleDuration;
  extern const float pistonDebatement;

  void addToList(RenderList & renderList, date birthDate, date deathDate, date period,
		 unsigned int length, Shader::id shaderId = Shader::default);
}

#endif		// ROTOR_HH
