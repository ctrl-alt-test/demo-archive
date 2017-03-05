//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

static int mirand  = 1;

void msys_srand( int semilla )
{
    mirand = semilla;
}

int msys_rand( void )
{
    mirand = mirand * 0x343FD + 0x269EC3;
    return( (mirand>>16)&32767 );
}


float msys_frand( void )
{
    //mirand = mirand * 0x343FD + 0x269EC3;
  /*
    mirand *= 16807;

    const unsigned int a = (mirand>>9) | 0x3f800000;
    const float res = (*((float*)&a)) - 1.0f;

    return( res );
    */
  return ((float)msys_rand()/32767);
}

float msys_sfrand( void )
{
    //mirand = mirand * 0x343FD + 0x269EC3;
/*
    mirand *= 16807;

    const unsigned int a = (mirand>>9) | 0x40000000;
    const float res = (*((float*)&a)) - 3.0f;

    return( res );
    */
  return ((float)(2*msys_rand())/32767)-1.f;
}

// Pure random (with seed in argument)
int msys_rand(int seed)
{
    seed += seed << 16; // FIXME: trouver une formule ailleurs qu'au hasard :)
    seed = seed * 0x343FD + 0x269EC3;
    int res = ((seed >> 16) & 32767);
    return res;
}

float msys_frand(int seed)
{
  return ((float)msys_rand(seed)/32767);
}

float msys_sfrand(int seed)
{
  return ((float)(2 * msys_rand(seed)) / 32767) - 1.f;
}
