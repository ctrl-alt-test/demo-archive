//
// Identifiants de toutes les animations utilisées
//

#ifndef		ANIM_ID_HH
# define	ANIM_ID_HH

namespace Anim
{
  enum id : unsigned char
    {
      none = 0,

      door,
      doorHandle,
      train,
      carouselPhysics,
      bookDominoFall,
      lastBookDominoFall,
      vibration,
      musicBoxRotation,
      musicBoxCylRotation,
      musicBoxOpening,
      pendulum,
      clockHand,
      leftArm,
      leftForearm,
      leftArmClap,
      leftForearmClap,
      xyloArm,
      xyloForearm,
      xyloStick,
      head,
      hihat,
      wheelsRotation,
      flute,
      lousianneRotate,
      lousianneRotateWheels,
      orgalame,
      orgalameBlades,
      pcBody,
      pcSlider,
      pcPipe,
      drumStick,

      numberOfAnims
    };
}

#endif		// ANIM_ID_HH
