//
// Ce qui touche à la caméra
//

#ifndef		CAMERA_HH
# define	CAMERA_HH

#include "anim.hh"

namespace Camera
{
  Anim::Anim * newAnimation();
  Anim::Anim * reloadAnimation();
  void lookAt(float *mtx, const vector3f &pos);
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
