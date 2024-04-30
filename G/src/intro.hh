//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

#include "sys/events.h"

#include "algebra/matrix.hh"
#include "anim.hh"
#include "basicTypes.hh"
#include "camera.hh"
#include "font.hh"
#include "fontsticker.hh"
#include "lensorbs.hh"
#include "loading.hh"
#include "renderlist.hh"
#include "shaderProgram.hh"
#include "timing.hh"
#include "variable.hh"

#define DEMO_TITLE "G - Level One"

#define MP3_LOAD_ERROR 1
#define MIDI_LOAD_ERROR 2

#if DEBUG

enum displayMode
  {
    displayScene = 0,
    displayBuffer,
    displayTexture,
    displayMaterial,
    displayMesh,

    numberOfDisplayModes
  };

enum renderMode
  {
    releaseRender = 0,
    whiteLightRender,
    normalsRender,
    lightingRender,

    numberOfRenderModes
  };

struct debugStuff
{
  renderMode	renderMode;
  displayMode	displayMode;
  Shader::compilResult shaderStatus;

  int   showTexture;
  int   showMaterial;
  int   showMesh;
  int   showBuffer;

  bool  lightingOnly;
  bool  light0;
  bool  light1;
  bool  ambient;
  bool  shadows;
  bool  lensEffects;

  bool  showProfiling;

  bool	nodesUpdate;
  bool	animation;
  bool	renderDrawcall;
  bool	shadowMapDrawcall;
  bool	applyShadowMapPostProcessing;
  bool	generateDownScale;
  bool	generateStreak;
  bool	generateGlow;
  bool	generateBokeh;
  bool	finalPass;
  bool	lensOrb;
  bool	displayOverlayText;
  bool	showCameraRepresentation;
};

#endif

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
  bool	showCredits;

  // FIXME : très bof d'avoir ça ici, c'est spécifique à chaque rendu
  float zNear;
  float zFar;

  RenderList * sceneRenderList;
  Anim::Anim<Camera::Camera> * cameraAnim;
  PostProcessing::LensOrbs * lensOrbs;

#if DEBUG
  Variables variables; // Toutes les grandeurs qui dépendent du temps

  debugStuff debug;

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

namespace Intro
{
  int init(int xres, int yres, float ratio, bool showCredits, Font::Font * font, Font::Sticker * sticker);
  void initSynchronization();
  int  mainLoop();
  void end();
  void updateTime();
}

#if DEBUG

void setDebugFlags(const char*);

void intro_key(KeyCode key, bool shift, bool ctrl);
void intro_mouse(const input_mouse & mouse, bool shift, bool ctrl);
void debugToolsInit();
void debugTools();
void debugActivateLightOptions();
void debugShowOverlayInfo();

void executeCommand(int command, const char* args);

#endif // DEBUG

#endif // _INTRO_H_
