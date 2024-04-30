//
// Ce qui touche à la caméra
//

#ifndef		CAMERA_HH
# define	CAMERA_HH

#include "algebra/vector3.hh"
#include "anim.hh"
#include "transformation.hh"

#define DEFAULT_FOV 39
#define DEFAULT_FOCUS 0.6f

namespace Camera
{
  struct Camera
  {
    Transformation t;
    float fov;
    float focus;
  };

  struct KeyframeSource
  {
    date dt;		// 4 octets
    unsigned char fov;	// 1
    char  qx;
    char  qy;
    char  qz;
    char  qw;		// 4
    float px;
    float py;
    float pz;		// 12
    unsigned char focus;	// 1
  };

  Camera interpolate(const Camera & c1,
		     const Camera & c2,
		     float weight);
  Camera interpolate(const Camera & c1,
		     const Camera & c2,
		     const Camera & c3,
		     const Camera & c4,
		     float weights[4]);

  Anim::Anim<Camera> * newAnimation();
  Anim::Anim<Camera> * reloadAnimation();

  Camera getCamera(date cameraDate);
  void placeCamera();
  void placeLightCamera();

  void orthoProj(double width, double height);

#if DEBUG

  void drawRepresentation(const Camera & camera);

#endif // DEBUG
}

// --------------------------------------------------------------------
// Future vraie caméra :
//
// Pour définir la caméra, il faut :
//  -une position
//  -une cible
//  -la position de la cible dans le champ de vision
//  -une deuxième cible
//  -la position de cette deuxième cible dans le champ de vision
//  - un angle de vision
//

// --------------------------------------------------------------------
// Conversions :
// Canon 400D -> full frame : x1.6
//
// fov = 2 * arctan( Di / (2 * focale) )
//
// fov : angle en degrés
// Di : diagonale du film en mm (24x36 : 43.27 ; 22.2x14.8 : 26.68 ? 26.80 ?)
// focale : focale  en mm
//
// Correspondances objectif - FOV full frame - FOV 400D
// 10 mm	130.39°	81°
// 18 mm	100.48°	62.4°
// 20 mm	94.5°	58.7°
// 28 mm	75.38°	46.8°
// 35 mm	63.45°	39.4°
// 50 mm	46.79°	29.1°
// 55 mm	42.95°	26.7°
// 70 mm	34.35°	21.3°
// 85 mm	28.56°	17.7°
// 100 mm	24.41°	15.16°
// 150 mm	16.41	10.2°
// 200 mm	12.35°	7.7°
// 300 mm	8.25°	5.1°
// 500 mm	4.95°	3.1°
//

#endif		// CAMERA_HH
