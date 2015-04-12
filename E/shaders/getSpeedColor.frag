varying vec3 vSpeed;

//
// Couleur représentative de la vitesse, pour la velocity map
//
// vSpeed contient la direction en xy, et une l'intensité en z
// (avec une correction type gamma pour avoir de la précision)
//
vec3 getSpeedColor()
{
  return vec3(0.5 + 0.5 * vSpeed.xy, pow(vSpeed.z, 0.5));
}
