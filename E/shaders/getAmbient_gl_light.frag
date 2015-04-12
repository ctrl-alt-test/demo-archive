//
// Lumière ambiante donnée par la GL_LIGHT
//
vec4 getAmbientColor()
{
  return gl_LightSource[0].ambient;
}
