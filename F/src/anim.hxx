//
// Animation
//

#ifndef		ANIM_HXX
# define	ANIM_HXX

#include "spline.h"

namespace Anim
{
  template<typename T>
  Anim<T>::Anim():
    _repeats(1), _dates(), _frames(), _cache()
  {}

  template<typename T>
  Anim<T>::Anim(int numberOfFrames, unsigned int repeats):
    _repeats(1), _dates(), _frames(), _cache()
  {
    setAnim(numberOfFrames, repeats);
  }

  template<typename T>
  Anim<T>::Anim(const T & start, const T & end, date duration):
    _repeats(1), _dates(2), _frames(2), _cache()
  {
    assert(start.date() < end.date());

    _dates.add(0);
    _dates.add(duration);
    _frames.add(start);
    _frames.add(end);
  }

  template<typename T>
  void Anim<T>::setAnim(int numberOfFrames, unsigned int repeats)
  {
    assert(numberOfFrames > 1);

    _repeats = repeats;
    _dates.init(numberOfFrames);
    _frames.init(numberOfFrames);
  }

#if DEBUG

  template<typename T>
  inline
  bool Anim<T>::hasFrames() const
  {
    return _dates.size > 0;
  }

#endif

  template<typename T>
  inline
  date Anim<T>::duration() const
  {
    IFDBG(assert(hasFrames()));

    return _dates.last();
  }

  template<typename T>
  inline
  void Anim<T>::add(const date date, const T & frame)
  {
    // Si c'est la première frame, la date doit être 0
    // Sinon la date doit être supérieure ou égale à la dernière
    IFDBG(assert((!hasFrames() && 0 == date) ||
	         date >= duration()));
    assert(!isCached()); // Le cache doit être créé en dernier

    _dates.add(date);
    _frames.add(frame);
  }

  template<typename T>
  inline
  bool Anim<T>::isCached() const
  {
    return _cache.elt != NULL;
  }

  template<typename T>
  void Anim<T>::createCache()
  {
    assert(!isCached()); // Le cache existait déjà
    IFDBG(assert(hasFrames()));

    _cache.init(duration());
    _cache.size = duration();
    for (int i = 0; i < _cache.size; ++i)
    {
      _cache[i].isComputed = false;
    }
  }

  template<typename T>
  T Anim<T>::get(date localDate)
  {
    if (_repeats != 0 && localDate / duration() >= _repeats)
    {
      return _frames.last();
    }

    const date seekDate = localDate % duration();
    if (isCached())
    {
      if (!_cache[seekDate].isComputed)
      {
	_cache[seekDate].element = _eval(seekDate);
	_cache[seekDate].isComputed = true;
      }
      return _cache[seekDate].element;
    }
    return _eval(seekDate);
  }

#if DEBUG

  template<typename T>
  const T & Anim<T>::getPrev(date localDate) const
  {
    if (_repeats != 0 && localDate / duration() >= _repeats)
    {
      return _frames.last();
    }

    const date seekDate = localDate % duration();

    int inf = 0;
    int sup = _dates.size - 1;
    while (inf < sup - 1)
    {
      const int med = (inf + sup) / 2;
      if (_dates[med] > seekDate)
      {
	sup = med;
      }
      else
      {
	inf = med;
      }
    }
    return _frames[inf];
  }

  template<typename T>
  const T & Anim<T>::getNext(date localDate) const
  {
    if (_repeats != 0 && localDate / duration() >= _repeats)
    {
      return _frames.last();
    }

    const date seekDate = localDate % duration();

    int inf = 0;
    int sup = _dates.size - 1;
    while (inf < sup - 1)
    {
      const int med = (inf + sup) / 2;
      if (_dates[med] > seekDate)
      {
	sup = med;
      }
      else
      {
	inf = med;
      }
    }
    return _frames[inf + 1];
  }

#endif // DEBUG

  template<typename T>
  T Anim<T>::_eval(date seekDate)
  {
    assert(seekDate <= duration());

    int inf = 0;
    int sup = _dates.size - 1;
    while (inf < sup - 1)
    {
      // Si ça claque ici, soit le tableau de dates est pourri,
      // soit je ne sais pas écrire une dicchotomie
      assert(_dates[inf] <= _dates[sup]);

      const int med = (inf + sup) / 2;
      if (_dates[med] > seekDate)
      {
	sup = med;
      }
      else
      {
	inf = med;
      }
    }
    return _eval(inf, seekDate);
  }

  template<typename T>
  T Anim<T>::_eval(int index, date seekDate) const
  {
    // FIXME : voir pour le type d'interpolation
    if (true)//keyframe1.transformation().use_spline == false)
    {
      const T & frame1 = _frames[index];
      const T & frame2 = _frames[index + 1];
      const date t1 = _dates[index];
      const date t2 = _dates[index + 1];
      const float weight = (float)(seekDate - t1) / (float)(t2 - t1);
      return interpolate(frame1, frame2, weight);
    }
    else
    {
      int indices[4];
      float dates[4];
      for (int i = index - 1; i <= index + 2; ++i)
      {
	int k = max(min(0, i), _dates.size - 1);
        if (k == index - 1 && _dates[k] == _dates[k + 1]) ++k;
        if (k == index + 2 && _dates[k] == _dates[k - 1]) --k;
	const T & frame = _frames[k];
	indices[i - (index - 1)] = k;
	dates[i - (index - 1)] = (float)_dates[k];
      }
      float weights[4];
      computeSplineWeights((float)seekDate, dates, weights);
      return interpolate(_frames[indices[0]],
			 _frames[indices[1]],
			 _frames[indices[2]],
			 _frames[indices[3]],
			 weights);
    }
  }
}

#endif		// ANIM_HXX
