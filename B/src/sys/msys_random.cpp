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