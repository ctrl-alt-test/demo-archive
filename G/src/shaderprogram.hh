//
// Shader program
//

#ifndef		SHADER_PROGRAM_HH
# define	SHADER_PROGRAM_HH

#include "array.hh"
#include "shaderid.hh"

namespace Shader
{
  enum compilResult // Par ordre de gravité
    {
      ok = 0,
      notReady,
      linkWarn,
      compilWarn,
      linkError,
      compilError,
    };

  class Program
  {
  public:
    Program();
    ~Program();

    Shader::id id() const;
    void setId(Shader::id id);
    unsigned int glId() const;
    bool isInUse() const;
    void use() const;

    static void unUse();

    void detachShader(unsigned int shader);
    void attachShader(unsigned int shader);
    void kill();

#if DEBUG
    void link(compilResult* result);
#else
    void link();
#endif

  private:
    Shader::id _id;
    unsigned int _gl_id;
    Array<unsigned int> _shaders;
  };
}

#endif		// SHADER_PROGRAM_HH
