//
// Coordonnées de texture
//
vec2 getTexCoord()
{
  return gl_TexCoord[0].st / gl_TexCoord[0].q;
}
