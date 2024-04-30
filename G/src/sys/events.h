//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _EVENTS_H_
#define _EVENTS_H_

enum KeyCode
{
  KEY_ARROW_LEFT = 0,
  KEY_ARROW_RIGHT,
  KEY_ARROW_UP,
  KEY_ARROW_DOWN,
  KEY_PGUP,
  KEY_PGDOWN,

  KEY_LSHIFT,
  KEY_RSHIFT,
  KEY_LCONTROL,
  KEY_RCONTROL,
  KEY_SPACE,
  KEY_RETURN,

  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,

  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,

  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,

  NUMBER_OF_KEYS
};

typedef struct
{
  bool state[NUMBER_OF_KEYS];
  //  KeyCode press[NUMBER_OF_KEYS];
} input_keyboard;

typedef struct
{
  int   dx, dy, dz;
  int	x, y, ox, oy;
  int	obuttons[2];
  int	buttons[2];
  int	dbuttons[2];
} input_mouse;

typedef struct
{
  input_keyboard	keyb;
  input_mouse		mouse;
} event_info_t;

event_info_t *getEvents( void );
int getKeyPress(KeyCode key);

#endif
