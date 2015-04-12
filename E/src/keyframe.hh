//
// Keyframe
//

#ifndef		KEYFRAME_HH
# define	KEYFRAME_HH

#include "transformation.hh"

namespace Anim
{
  class Keyframe
  {
  public:
    Keyframe(float date, float size = 1.f,
	     bool use_spline = false, float textureFade = 0);
    Keyframe(float date, const Transformation & transformation);
    ~Keyframe();

    float			date() const;
    const Transformation &	transformation() const;

  private:
    float		_date;
    Transformation	_transformation;
  };
}


#include "keyframe.hxx"

#endif		// KEYFRAME_HH
