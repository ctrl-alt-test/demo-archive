//
// Animation
//

#ifndef		ANIM_HH
# define	ANIM_HH

#include "array.hh"
#include "basicTypes.hh"
#include "keyframe.hh"
#include "transformation.hh"

namespace Anim
{
  typedef struct
  {
    bool computed;
    Transformation t;
  } cachedElement;

  class Anim : public Array<Keyframe>
  {
  public:
    Anim();
    Anim(int keyframes, date duration, unsigned char repeats = 1);
    Anim(const Keyframe & start, const Keyframe & end, date duration);
    ~Anim();

    void setAnim(int keyframes, date duration, unsigned char repeats = 1);
    void normalize();

    /*
    static Anim * newPushPullCycle(date duration, float length);
    static Anim * newSquareCycle(date duration, float length, float width, bool vertical = false);
    static Anim * newCubeValidatorCycle(date duration, float size);
    static Anim * newSphereDieAnim(date duration);
    */

#if DEBUG

    void add(const Keyframe & item);
    void addUnsafe(const Keyframe & item);

#endif // DEBUG

    date		duration() const;
    unsigned char	repeats() const;

    Transformation	getTransformation(date localDate, float shift = 0) const;
    Transformation	cachedGetTransformation(date localDate, float shift = 0);

    bool		cached() const;
    void		createCache();

#if DEBUG

    const Transformation & getPrevTransformation(date localDate, float shift) const;
    const Transformation & getNextTransformation(date localDate, float shift) const;

#endif // DEBUG

  private:
    Transformation	_getTransformation(int index, float progression) const;
    Transformation      _getTransformationSpline(int index, float progression) const;
    date		_duration;
    float		_invDuration;
    unsigned char	_repeats; // 0 -> infini
    cachedElement *	_cache;
  };

  Transformation getMixedTransformation(date date1, date date2, Anim &anim1, Anim &anim2, float weight);
}

#ifdef DEBUG
# define AnimAddUnsafe(a, k) a.addUnsafe(k)
#else
# define AnimAddUnsafe(a, k) a.add(k)
#endif

#include "anim.hxx"

#endif		// ANIM_HH
