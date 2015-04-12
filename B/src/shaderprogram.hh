//
// Shader program
//

#ifndef		SHADER_PROGRAM_HH
# define	SHADER_PROGRAM_HH

namespace Shader
{
  class Program
  {
  public:
    Program();
    ~Program();

    unsigned int id() const;
    void use() const;

    static void unUse();

    void detachVertexShader();
    void attachVertexShader(unsigned int vertexShader);
    void detachFragmentShader();
    void attachFragmentShader(unsigned int fragmentShader);

    void LoadShaders(const char * vertex, const char * fragment);

  private:
    unsigned int _id;
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
  };
}


#include "shaderprogram.hxx"

#endif		// SHADER_PROGRAM_HH
