#include "Rand.hh"

using namespace Noise;

int Rand::igen()
{
	// FIXME : ce générateur de bruit est très pauvre, or on l'utilise
	//         absolument partout. On pourrait peut-être investir dans
	//         un meilleur générateur ? Par exemple xorshift64*.

	_seed = _seed * 0x343FD + 0x269EC3;
	return ((_seed >> 16) & 32767);
}

// xorshift64*:
// Written in 2014 by Sebastiano Vigna (vigna@acm.org), public domain.
// This is a fast, good generator if you're short on memory, but otherwise
// we rather suggest to use a xorshift128+ or xorshift1024* (for a very
// long period) generator.
//
//  uint64_t x; /* The state must be seeded with a nonzero value. */
//  
//  uint64_t next() {
//  	x ^= x >> 12; // a
//  	x ^= x << 25; // b
//  	x ^= x >> 27; // c
//  	return x * 2685821657736338717LL;
//  }
