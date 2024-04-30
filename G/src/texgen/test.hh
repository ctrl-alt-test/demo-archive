// ============================================================================
//
// test.hh for Texgen: made by Zavie (Julien Guertault)
//
// ============================================================================

#ifndef		TEST_HH
# define	TEST_HH

#include		"texture.hh"

void		RAW(const Texture::Channel & r,
		    const Texture::Channel & g,
		    const Texture::Channel & b,
		    const std::string& file);
void		RAW(const Texture::Channel & a,
		    const std::string& file);

void		TestTexture ();

#endif		// TEST_HH
