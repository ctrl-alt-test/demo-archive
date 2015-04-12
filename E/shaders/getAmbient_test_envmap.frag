vec3 nNormal;

//
// Lumière ambiante de test dépendante de la direction
//
vec4 getAmbientColor()
{
  return vec4(0.2 + 0.2 * nNormal, 1.);
}
