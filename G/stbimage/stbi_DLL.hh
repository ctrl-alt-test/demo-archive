
#ifndef		STBI_DLL_HH
# define	STBI_DLL_HH

namespace STB
{
  class STBI
  {
  public:

    // load image from memory buffer
    static __declspec(dllexport) unsigned char * __fastcall LoadFromMemory(unsigned char const *buffer, int len, int *x, int *y, int *comp, int req_comp);

    // load image by filename
    static __declspec(dllexport) unsigned char * __fastcall Load(char const *filename, int *x, int *y, int *comp, int req_comp);

    // free the loaded image -- this is just free()
    static __declspec(dllexport) void __fastcall Free(void *retval_from_stbi_load);
  };
}

#endif // STBI_DLL_HH
