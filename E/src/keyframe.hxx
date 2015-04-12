//
// Keyframe
//

#ifndef		KEYFRAME_HXX
# define	KEYFRAME_HXX

#define INLINE_ACCESSOR(t, x) inline t Keyframe::x() const {return _##x;}

namespace Anim
{
  INLINE_ACCESSOR(float, date);
  INLINE_ACCESSOR(const Transformation &, transformation);
}

#undef INLINE_ACCESSOR

#endif		// KEYFRAME_HXX
