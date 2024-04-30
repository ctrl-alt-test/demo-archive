// ============================================================================
//
// Texture generator tests
//
// ============================================================================

#include <iostream>
#include <string>
#include <cstdio>

#include "texture.hh"
#include "interpolation.hxx"

// FIXME: static variable - might lead to race conditions
static unsigned char data[4 * 1024*1024];

static unsigned char floatToByte(float value)
{
  return (unsigned char)(clamp(value) * 255);
}

//
// Y axis up side down for OpenGL way of handling texture coordinates
//
const unsigned char * AlphaBuffer(const Texture::Channel & a)
{
  const unsigned int size = a.Width() * a.Height();
  for (unsigned int i = 0; i < size; ++i)
    {
      data[i] = floatToByte(a[i]);
    }
  return data;
}

const unsigned char * RGBBuffer(const Texture::Channel & r,
                                const Texture::Channel & g,
                                const Texture::Channel & b)
{
  const unsigned int size = r.Width() * r.Height();
  for (unsigned int i = 0; i < size; ++i)
    {
      data[3 * i] = floatToByte(r[i]);
      data[3 * i + 1] = floatToByte(g[i]);
      data[3 * i + 2] = floatToByte(b[i]);
    }
  return data;
}

const unsigned char * RGBABuffer(const Texture::Channel & r,
                                 const Texture::Channel & g,
                                 const Texture::Channel & b,
                                 const Texture::Channel & a)
{
  const unsigned int size = r.Width() * r.Height();
  for (unsigned int i = 0; i < size; ++i)
    {
      data[4 * i] = floatToByte(r[i]);
      data[4 * i + 1] = floatToByte(g[i]);
      data[4 * i + 2] = floatToByte(b[i]);
      data[4 * i + 3] = floatToByte(a[i]);
    }
  return data;
}


void            RAW(const Texture::Channel & r,
                    const Texture::Channel & g,
                    const Texture::Channel & b,
                    const std::string& file)
{
  const unsigned int size = 3 * r.Width() * r.Height();
  const unsigned char * render = RGBBuffer(r, g, b);

  FILE *fd = fopen(file.c_str(), "ab");
  if (0 == fd)
    return;

  if (size != fwrite(render, 1, size, fd))
    std::cerr << "Failed to dump texture! T_T" << std::endl;
  fclose (fd);
}

// Methode un peu space...
void            RAW(const Texture::Channel & a, const std::string& file)
{
  const unsigned int size = 3 * a.Width() * a.Height();
  const unsigned char * render = RGBBuffer(a, a, a);

  FILE *fd = fopen(file.c_str(), "ab");
  if (0 == fd)
    return;

  if (size != fwrite(render, 1, size, fd))
    std::cerr << "Failed to dump texture! T_T" << std::endl;

  fclose (fd);
}
