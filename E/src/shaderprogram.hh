//
// Shader program
//

#ifndef		SHADER_PROGRAM_HH
# define	SHADER_PROGRAM_HH

#include "array.hh"

namespace Shader
{
  class Program
  {
  public:
    Program();
    ~Program();

    unsigned int id() const;
    bool isInUse() const;
    void use() const;

    static void unUse();

    void detachShader(unsigned int shader);
    void attachShader(unsigned int shader);

    void link();

  private:
    unsigned int _id;
    Array<unsigned int> _shaders;
  };
}


#include "shaderprogram.hxx"

#endif		// SHADER_PROGRAM_HH
