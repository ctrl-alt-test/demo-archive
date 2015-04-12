//
// L system alphabet
//

#ifndef		L_SYSTEM_ALPHABET_HH
# define	L_SYSTEM_ALPHABET_HH

#define ALPHABET ".+-&^<>[]XFGH"

namespace LSystem
{
  typedef enum
    {
      DUMMY = 0,	// '\0' : fin de chaîne
      LEFT = 1,		// +
      RIGHT = 2,	// -
      DOWN = 3,		// &
      UP = 4,		// ^
      COUNTERCLOCK = 5,	// <
      CLOCK = 6,	// >
      PUSH = 7,		// [
      POP = 8,		// ]
      FORWARD1 = 9,	// X
      FORWARD2 = 10,	// F
      FORWARD3 = 11,	// G
      FORWARD4 = 12,	// H
      ALPHABET_LENGTH = 13
    } alphabetElement;
}

#endif		// L_SYSTEM_ALPHABET_HH
