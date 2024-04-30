
#include "shaderuniform.hh"
#include "sys/msys_glext.h"

ShaderUniform::ShaderUniform():
  _id(-1), _name(NULL)
{
}

ShaderUniform::ShaderUniform(int id,
			     dataType dataType,
			     const char * name
			     DBGARG(float min)
			     DBGARG(float max)):
  _id(id), _name(name), _dataType(dataType)
{
#if DEBUG
  _min = min;
  _max = max;
#endif // DEBUG

  for (int i = 0; i < 16; ++i)
  {
    _valuei[i] = 0;
  }
}

int ShaderUniform::id() const
{
  return _id;
}

const char * ShaderUniform::name() const
{
  return _name;
}

const ShaderUniform::dataType ShaderUniform::type() const
{
  return _dataType;
}

const float * ShaderUniform::valuef() const
{
  return _valuef;
}

const int * ShaderUniform::valuei() const
{
  return _valuei;
}

void ShaderUniform::set(int value)
{
  assert(_dataType == vec1i);
#if DEBUG
  assert(value >= _min && value <= _max);
#endif // DEBUG

  _valuei[0] = value;
}

void ShaderUniform::set(float value)
{
  assert(_dataType == vec1);
#if DEBUG
  assert(value >= _min && value <= _max);
#endif // DEBUG

  _valuef[0] = value;
}

void ShaderUniform::set(const vector2f& value)
{
  assert(_dataType == vec2);
 #if DEBUG
  assert(value.x >= _min && value.x <= _max &&
	 value.y >= _min && value.y <= _max);
#endif // DEBUG

  _valuef[0] = value.x;
  _valuef[1] = value.y;
}

void ShaderUniform::set(const vector3f& value)
{
  assert(_dataType == vec3);
#if DEBUG
  assert(value.x >= _min && value.x <= _max &&
	 value.y >= _min && value.y <= _max &&
	 value.z >= _min && value.z <= _max);
#endif // DEBUG

  _valuef[0] = value.x;
  _valuef[1] = value.y;
  _valuef[2] = value.z;
}

void ShaderUniform::set(const vector4f& value)
{
  assert(_dataType == vec4);
#if DEBUG
  assert(value.x >= _min && value.x <= _max &&
	 value.y >= _min && value.y <= _max &&
	 value.z >= _min && value.z <= _max &&
	 value.w >= _min && value.w <= _max);
#endif // DEBUG

  _valuef[0] = value.x;
  _valuef[1] = value.y;
  _valuef[2] = value.z;
  _valuef[3] = value.w;
}

void ShaderUniform::set(const matrix4& value)
{
  assert(_dataType == mat4);

  for (int i = 0; i < 16; ++i)
  {
    _valuef[i] = value.m[i];
  }
}

void ShaderUniform::send(int location DBGARG(int shaderId))
{
  switch (_dataType)
  {
  case vec1: oglUniform1f(location, _valuef[0]); break;
  case vec2: oglUniform2fv(location, 1, _valuef); break;
  case vec3: oglUniform3fv(location, 1, _valuef); break;
  case vec4: oglUniform4fv(location, 1, _valuef); break;

  case vec1i: oglUniform1i(location, _valuei[0]); break;

  case mat4: oglUniformMatrix4fv(location, 1, 0, _valuef); break;

  case vec2i: // glUniform2iv(location, 1, valuei); break;
  case vec3i: // glUniform3iv(location, 1, valuei); break;
  case vec4i: // glUniform4iv(location, 1, valuei); break;
  case mat2: // glUniformMatrix2fv(location, count, const float *value);
  case mat3: // glUniformMatrix3fv(location, count, const float *value);
  default:
    assert(false);
  }
  OGL_ERROR_CHECK("ShaderUniform::send %s to shader %d", _name, shaderId);
}

void ShaderUniform::get(int location, unsigned int shaderGlId)
{
  switch (_dataType)
  {
  case vec1:
  case vec2:
  case vec3:
  case vec4:
  case mat2:
  case mat3:
  case mat4:
    ((PFNGLGETUNIFORMFVPROC)(wglGetProcAddress("glGetUniformfv")))(shaderGlId, location, _valuef);
    break;

  case vec1i:
  case vec2i:
  case vec3i:
  case vec4i:
    ((PFNGLGETUNIFORMIVPROC)(wglGetProcAddress("glGetUniformiv")))(shaderGlId, location, _valuei);
    break;

  default:
    assert(false);
  }
  OGL_ERROR_CHECK("ShaderUniform::get %s from shader %d", _name, shaderGlId);
}
