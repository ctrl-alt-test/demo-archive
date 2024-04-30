#version 120

vec3 nNormal;

//
// Lumière ambiante de test dépendante de la direction
//
vec3 getAmbientColor()
{
  return vec3(0.2 + 0.2 * nNormal);
}
