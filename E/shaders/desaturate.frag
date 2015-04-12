
//
// Saturation entre 0 et la couleur originale
//
vec3 desaturate(vec3 color, float saturation)
{
  // FIXME : voir pour une équation plus rigoureuse de l'intensité
  float intensity = length(color);
  return mix(vec3(intensity), color.rgb, saturation);
}

/*
vec3 saturate(vec3 col, float n)
{
  float grey = (col.r + col.g + col.b) / 3.;
  col.r += (col.r - grey) * n;
  col.g += (col.g - grey) * n;
  col.b += (col.b - grey) * n;
  return col;
}
*/

// Ca pourrait être mieux d'avoir une fonction saturate entre -1 et 1 :
// -1 : désaturé
// 0 : identique
// 1 : saturé
// Voir comment c'est dans les logiciels de retouche d'image
