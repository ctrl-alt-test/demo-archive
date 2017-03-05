//
// Animation
//

#ifndef		ANIM_HH
# define	ANIM_HH

#include "array.hh"
#include "basicTypes.hh"
#include "transformation.hh"

namespace Anim
{
  // Anim définit une animation sur un type T (flottant, vecteur,
  // quaternion...)
  // Il faut que soient définies les fonctions
  // T interpolate(T, T, float)
  // T interpolate(T, T, T, T, float[4])
  template<typename T>
  struct cached
  {
    bool isComputed;
    T element;
  };

  template<typename T>
  class Anim
  {
  public:
    Anim();
    Anim(int numberOfFrames, unsigned int repeats = 1);
    Anim(const T & start, const T & end, date duration);
    ~Anim() {};

    void setAnim(int numberOfFrames, unsigned int repeats = 1);

    IFDBG(bool hasFrames() const;)

    date duration() const;
    void add(const date date, const T & frame);

    bool isCached() const;
    void createCache();

    T get(date localDate);

#if DEBUG

    const T & getPrev(date localDate) const;
    const T & getNext(date localDate) const;

#endif // DEBUG

  private:
    T _eval(date localDate);
    T _eval(int index, date seekDate) const;

    unsigned int	_repeats; // 0 => infini

    Array<date>		_dates;
    Array<T>		_frames;
    Array<cached<T> >	_cache;
  };
}

#include "anim.hxx"

#endif		// ANIM_HH
