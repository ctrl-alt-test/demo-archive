//
// The loading part
//

#ifndef		LOADING_HH
# define	LOADING_HH

#include "basicTypes.hh"

namespace Loading
{
  void init();
  void setupRendering();
  int draw();
  void drawPostLoaded(date renderDate);

  void clearScreen(); // Fonction à appeler une fois, avant toute chose
  void update();      // Fonction à appeler régulièrement pendant le chargement
  void drawLoading(float step);
}

#define POST_LOAD_DURATION 500

#endif		// LOADING_HH
