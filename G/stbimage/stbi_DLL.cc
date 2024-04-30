
#include "stbi_DLL.hh"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.cc"

using namespace STB;

unsigned char * STBI::LoadFromMemory(unsigned char const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
  return stbi_load_from_memory(buffer, len, x, y, comp, req_comp);
}

unsigned char * STBI::Load(char const *filename, int *x, int *y, int *comp, int req_comp)
{
  return stbi_load(filename, x, y, comp, req_comp);
}

void STBI::Free(void *retval_from_stbi_load)
{
  stbi_image_free(retval_from_stbi_load);
}
