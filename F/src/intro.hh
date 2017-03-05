//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

#include "sys/events.h"

#include "anim.hh"
#include "basicTypes.hh"
#include "camera.hh"
#include "font.hh"
#include "fontsticker.hh"
#include "lensorbs.hh"
#include "loading.hh"
#include "renderlist.hh"
#include "timing.hh"

#define DEMO_TITLE "F - Felix's workshop"

struct IntroObject
{
  int	xres;          // Resolution x de l'écran
  int	yres;          // Resolution y de l'écran
  float aspectRatio;   // Rapport largeur/hauteur de l'écran

  int	yresVP;        // Resolution y du viewport
  int	yVP;           // Position y du viewport
  float aspectRatioVP; // Rapport largeur/hauteur du viewport

  long	mTo;
  Clock now;

  Font::Font * font;
  Font::Sticker * sticker;
  int	textHeight;

  // FIXME : très bof d'avoir ça ici, c'est spécifique à chaque rendu
  float zNear;
  float zFar;

  RenderList * sceneRenderList;
  Anim::Anim<Camera::Camera> * cameraAnim;
  PostProcessing::LensOrbs * lensOrbs;

#if DEBUG

  int   showBuffer;
  int   showTexture;
  bool  showProfiling;
  int   debugRenderMode;
  date  startTime;

  bool  manualCamera;
  matrix4 cameraPosition;
  unsigned char fov;
  float focus;

  bool initDone; // passe à vrai une fois que le chargement est fait

  bool  manualLight;
  float lightPosition[4];

  bool sound;

#endif // DEBUG
};

extern IntroObject intro;

void intro_init(int xres, int yres, float ratio, bool music, Font::Font * font, Font::Sticker * sticker);
void intro_init_synchronization();
int  intro_do();
void intro_end();

#if DEBUG

void intro_key(int key, bool shift, bool ctrl);
void intro_mouse(const input_mouse & mouse, bool shift, bool ctrl);
void intro_updateTime();
void debugToolsInit();
void debugTools();
void debugShowTexture();
void debugShowBuffer();
void debugShowRenderableList();

#endif // DEBUG

#endif // _INTRO_H_
