//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#include <math.h>
#include "mzk.h"

// put here your synth
void mzk_init( short *buffer )
{
    for( int i=0; i<MZK_NUMSAMPLES; i++ ) 
    {
        const float fl = sinf( 6.2831f*440.0f * (float)i/(float)MZK_RATE );
        const float fr = sinf( 6.2831f*587.3f * (float)i/(float)MZK_RATE );

        buffer[2*i+0] = (int)(fl*32767.0f);
        buffer[2*i+1] = (int)(fr*32767.0f);
    }
}
