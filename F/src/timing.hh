//
// Définition des temps absolus en ms pour le déroulement
//

#ifndef TIMING_HH
#define TIMING_HH

#include "basicTypes.hh"

struct Clock
{
  date	story;		// Date dans l'histoire
  date	camera;		// Date dans le rush (montage en vitesse 1)
  date	youtube;	// Date dans la vidéo finale de la démo

#if DEBUG

  date	real;		// Temps physique depuis le lancement de l'exécutable

  date	startTime;
  date	pauseDate;
  int	pauseShift;
  bool	paused;

#endif // DEBUG

};

extern const date worldStartDate;
extern const date worldEndDate;

extern const date demoEndDate;

#endif // !TIMING_HH
