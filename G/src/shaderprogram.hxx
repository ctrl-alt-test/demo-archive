//
// Shader program
//

#ifndef		SHADER_PROGRAM_HXX
# define	SHADER_PROGRAM_HXX

namespace Shader
{
  inline
  id Program::id() const
  {
    return _id;
  }

  inline
  void Program::setId(Shader::id id)
  {
    _id = id;
  }

  inline
  unsigned int Program::glId() const
  {
    return _gl_id;
  }
}

#endif		// SHADER_PROGRAM_HXX
