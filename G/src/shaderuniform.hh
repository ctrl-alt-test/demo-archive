#ifndef SHADER_UNIFORM_HH
#define SHADER_UNIFORM_HH

#include "algebra/matrix.hh"
#include "algebra/vector2.hh"
#include "algebra/vector3.hh"
#include "algebra/vector4.hh"
#include "sys/msys_debug.h"

class ShaderUniform
{
public:
  enum dataType
    {
      vec1,
      vec2,
      vec3,
      vec4,

      vec1i,
      vec2i,
      vec3i,
      vec4i,

      mat2,
      mat3,
      mat4,
    };

  ShaderUniform();

  ShaderUniform(int id,
		dataType dataType,
		const char * name
		DBGARG(float min)
		DBGARG(float max));

  int id() const;
  const char * name() const;
  const dataType type() const;

  const float * valuef() const;
  const int * valuei() const;
  void set(int value);
  void set(float value);
  void set(const vector2f& value);
  void set(const vector3f& value);
  void set(const vector4f& value);
  void set(const matrix4& value);

  void send(int location DBGARG(int shaderId));
  void get(int location, unsigned int shaderGlId);

  const float* getf() const { return _valuef; };

private:
  int		_id;
  const char *	_name;
  dataType	_dataType;

  union
  {
    float	_valuef[16]; // Au plus, une matrice 4x4
    int		_valuei[16];
  };

#if DEBUG
  float		_min;
  float		_max;
#endif // DEBUG
};

#endif // SHADER_UNIFORM_HH
