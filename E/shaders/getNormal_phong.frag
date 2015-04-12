varying vec3 vNormal;

//
// Normale interpolée suivant le modèle de Phong
//
vec3 getNormal()
{
  return normalize(vNormal);
}
