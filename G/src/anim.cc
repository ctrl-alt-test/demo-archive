//
// Animation
//

#include "anim.hh"

#include "basicTypes.hh"
#include "spline.h"
#include "transformation.hh"

#include "sys/msys_libc.h"
#include "sys/msys_malloc.h"

#define DEBUG_DISACTIVATE_ANIM_INTERPOLATION 1


namespace Anim
{
  Anim::Anim():
    Array<Keyframe>(),
    _duration(0),
    _invDuration(1.f),
    _repeats(1),
    _cache(NULL)
  {
  }

  Anim::Anim(int keyframes, date duration, unsigned char repeats):
    Array<Keyframe>(keyframes),
    _duration(duration),
    _invDuration(1.f / duration),
    _repeats(repeats),
    _cache(NULL)
  {
    assert(keyframes > 1);
    assert(duration > 0);
  }

  Anim::Anim(const Keyframe & start, const Keyframe & end, date duration):
    Array<Keyframe>(2),
    _duration(duration),
    _invDuration(0 == duration ? 1.f : 1.f / duration),
    _repeats(1),
    _cache(NULL)
  {
    assert(start.date() < end.date());
    add(start);
    add(end);
  }

  Anim::~Anim()
  {
    msys_mallocFree(_cache);
  }


  void Anim::setAnim(int keyframes, date duration, unsigned char repeats)
  {
    assert(keyframes > 1);
    assert(duration > 0);

    init(keyframes); // Array::init
    _duration = duration;
    _invDuration = 1.f / duration;
    _repeats = repeats;
  }

#if DEBUG

  void Anim::add(const Keyframe & item)
  {
    assert(item.date() >= 0 && item.date() <= 1.f);
    assert(size == 0 || item.date() >= last().date());
    Array<Keyframe>::add(item);
  }

  void Anim::addUnsafe(const Keyframe & item)
  {
    assert(item.date() >= 0);
    assert(size == 0 || item.date() >= last().date());
    Array<Keyframe>::add(item);
  }

#endif // DEBUG

  void Anim::normalize()
  {
    //
    // FIXME : systématiser normalize, car placer les temps à la main
    // c'est chiant ;
    // au lieu de ça on met les temps en absolu, on note la durée, et
    // on normalise.
    //
    const float length = last().date();
    assert(length > 1.f); // si length == 1.f, le normalize est inutile
    for(int i = 0; i < size; i++)
      elt[i] = Keyframe(elt[i].date() / length, elt[i].transformation());
  }

  // Interpolation de l'animation avec une spline
  Transformation Anim::_getTransformationSpline(int index, float progression) const
  {
    // dans le tableau : 4 * (t, s=fov, px, py, pz) flottants.
    float keys[4 * 5];
    float *pos = keys;
    for (int i = index-1; i <= index+2; i++)
      {
	int k = (i < 0 ? 0 : i);
	k = (k >= size ? size - 1 : k);
        if (k == index - 1 && elt[k].date() == elt[k+1].date()) k++;
        if (k == index + 2 && elt[k].date() == elt[k-1].date()) k--;
	const Transformation & t = elt[k].transformation();
	*pos++ = elt[k].date();
	*pos++ = t.s;
	*pos++ = t.v.x;
	*pos++ = t.v.y;
	*pos++ = t.v.z;
      }
    float res[4];
    spline4(keys, 4, progression, res);
    Transformation t;
    t.use_spline = false;
    t.dof = DEFAULT_DOF;
    t.s = res[0];
    t.v.x = res[1];
    t.v.y = res[2];
    t.v.z = res[3];
    return t;
  }

  //
  // It is assumed that there is a frame at 0 and a frame at 1
  //
  Transformation Anim::_getTransformation(int index, float progression) const
  {
    const Keyframe & keyframe1 = elt[index];
    const Keyframe & keyframe2 = elt[(index + 1) % size];
    const float duration = keyframe2.date() - keyframe1.date();
    const float weight = (progression - keyframe1.date()) / duration;
    Transformation t;
    const Transformation t1 = keyframe1.transformation();
    const Transformation t2 = keyframe2.transformation();
    if (keyframe1.transformation().use_spline == false)
    {
      t = transformationInterpolation(t1, t2, weight);
    }
    else
    {
      t = _getTransformationSpline(index, progression);
    }
    t.q = slerp(t1.q, t2.q, weight);
    t.dof = t1.dof + weight * (t2.dof - t1.dof);
    t.textureFade = t1.textureFade + weight * (t2.textureFade - t1.textureFade);
    return t;
  }

  Transformation Anim::getTransformation(date localDate, float shift) const
  {
    const float progression = shift + localDate * _invDuration;
    assert(_invDuration > 0.f);
    assert(_invDuration <= 1.f);
    assert(shift >= 0.f);
    assert(progression >= 0.f);
    const int finishedCycles = msys_ifloorf(progression);
    assert(finishedCycles <= progression);
    const int last = size - 1;

    if (_repeats != 0 && finishedCycles >= _repeats)
    {
      // Dernière keyframe
      return elt[last].transformation();
    }
    else
    {
      const float cycleProgression = progression - finishedCycles;
      assert(cycleProgression >= 0.f);
      assert(cycleProgression <= 1.f);
      int inf = 0;
      int sup = last;
      while (inf < sup - 1)
      {
        assert(elt[inf].date() <= elt[sup].date());
	const int med = (inf + sup) / 2;
	if (elt[med].date() > cycleProgression)
	{
	  sup = med;
	}
	else
	{
	  inf = med;
	}
      }
      return _getTransformation(inf, cycleProgression);
    }
  }

  Transformation Anim::cachedGetTransformation(date localDate, float shift)
  {
    assert(_cache != NULL); // Il faut d'abord créer le cache
    assert(localDate < _duration); // L'objet aurait dû être supprimé

    if (!_cache[localDate].computed)
    {
      _cache[localDate].t = getTransformation(localDate, shift);
      _cache[localDate].computed = true;
    }
    return _cache[localDate].t;
  }

  bool Anim::cached() const
  {
    return _cache != NULL;
  }

  void Anim::createCache()
  {
    assert(NULL == _cache); // Le cache existait déjà
    _cache = (cachedElement *) msys_mallocAlloc(_duration * sizeof cachedElement);
  }

  Transformation getMixedTransformation(date date1, date date2,
					Anim &anim1, Anim &anim2,
					float weight)
  {
    Transformation t1 = anim1.getTransformation(date1, 0.f);
    Transformation t2 = anim2.getTransformation(date2, 0.f);
    Transformation t = transformationInterpolation(t1, t2, weight);

    t.q = slerp(t1.q, t2.q, weight);
    t.dof = t1.dof + weight * (t2.dof - t1.dof);
    t.textureFade = t1.textureFade + weight * (t2.textureFade - t1.textureFade);
    return t;
  }

  //---------------------------------------------------------------------

#if DEBUG

  // FIXME : essayer de factoriser éventuellement

  const Transformation & Anim::getPrevTransformation(date localDate, float shift) const
  {
    assert(localDate < _duration); // L'objet aurait dû être supprimé

    const float progression = shift + localDate * _invDuration;
    const int finishedCycles = msys_ifloorf(progression);
    const int last = size - 1;

    if (_repeats != 0 && finishedCycles > _repeats)
    {
      // Dernière keyframe
      return elt[last].transformation();
    }
    else
    {
      const float cycleProgression = progression - finishedCycles;
      int inf = 0;
      int sup = last;
      while (inf < sup - 1)
      {
	const int med = (inf + sup) / 2;
	if (elt[med].date() > cycleProgression)
	{
	  sup = med;
	}
	else
	{
	  inf = med;
	}
      }
      return elt[inf].transformation();
    }
  }

  const Transformation & Anim::getNextTransformation(date localDate, float shift) const
  {
    assert(localDate < _duration); // L'objet aurait dû être supprimé

    const float progression = shift + localDate * _invDuration;
    const int finishedCycles = (int)progression;
    const int last = size - 1;

    if (_repeats != 0 && finishedCycles > _repeats)
    {
      // Dernière keyframe
      return elt[last].transformation();
    }
    else
    {
      const float cycleProgression = progression - finishedCycles;
      int inf = 0;
      int sup = last;
      while (inf < sup - 1)
      {
	const int med = (inf + sup) / 2;
	if (elt[med].date() > cycleProgression)
	{
	  sup = med;
	}
	else
	{
	  inf = med;
	}
      }
      return elt[inf + 1].transformation();
    }
  }

#endif // DEBUG

}
