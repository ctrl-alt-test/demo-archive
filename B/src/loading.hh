//
// The loading part
//

#ifndef		LOADING_HH
# define	LOADING_HH

#include "basicTypes.hh"

#define LOADING_STEPS	200
#define POST_LOAD_DURATION 2000

namespace Loading
{
  typedef struct
  {
    void *obj;
    void (*func)( void *data, int n );  // n=[0..LOADING_STEPS]
  } ProgressDelegate;

  void setupRendering(int xr, int yr);
  void clearRendering(date renderDate);
  void draw(int n);
  void drawPostLoaded(date renderDate);
}

#endif		// LOADING_HH
