//
// Dust on the lens
//

#ifndef		LENSORBS_HH
# define	LENSORBS_HH

#include "array.hh"
#include "vector.hh"

namespace PostProcessing
{
  class LensOrbs
  {
  public:
    LensOrbs();
    ~LensOrbs();

    void apply();

  private:
    Array<vector3f> _points;
  };
}

#endif		// LENSORBS_HH
