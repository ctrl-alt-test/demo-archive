//
// Animation
//

#ifndef		ANIM_HXX
# define	ANIM_HXX

#define INLINE_ACCESSOR(t, x) inline t Anim::x() const {return _##x;}

namespace Anim
{
  INLINE_ACCESSOR(date, duration);
  INLINE_ACCESSOR(unsigned char, repeats);
}

#undef INLINE_ACCESSOR

#endif		// ANIM_HXX
