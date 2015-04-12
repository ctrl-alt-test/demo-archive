//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

#include "sys/events.h"
#include "anim.hh"
#include "basicTypes.hh"
#include "font.hh"
#include "loading.hh"
#include "renderlist.hh"
#include "timing.hh"

typedef struct
{
  int	xres;
  int	yres;

  long	mTo;
  Clock now;

  Font * font;
  int	textHeight;

  float zNear;
  float zFar;

  RenderList * sceneRenderList;
/*   RenderList * trailsRenderList; */
  Anim::Anim * cameraAnim;

#if DEBUG

  date  startTime;

  bool  manualCamera;
  float	cameraPosition[16];
  unsigned char fov;
  float dof;

  bool  manualLight;
  float lightPosition[4];

#endif // DEBUG
} IntroObject;

#if DEBUG

extern IntroObject intro; // En globale en DEBUG, pour intro_debug_controls

#endif // DEBUG

int  intro_init( int xres, int yres, bool music, Font * font,
		 Loading::ProgressDelegate * pd );
void intro_init_synchronization();
int  intro_do( void );
void intro_key(int key, bool shift, bool ctrl);
void intro_mouse(const input_mouse & mouse, bool shift, bool ctrl);
void intro_updateTime();
void intro_end( void );
void debugToolsInit();

#endif
