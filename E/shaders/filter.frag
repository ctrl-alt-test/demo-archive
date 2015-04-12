//
// Filtre la couleur suivant les arguments
//
// Exemple d'utilisation :
// color = filter(color, vec3(1.), vec3(0.), vec3(1.));
//
vec4 filter(vec4 color, vec3 low, vec3 hig, vec3 gamma)
{
  return vec4(pow(clamp(mix(low, hig, color.rgb), 0., 1.), gamma), color.a);
}

//
// Note concernant le gamma :
//
// - l'éclairage doit être fait en espace linéaire (avant correction)
// - en R8G8B8 la précision est insuffisante et ça crée des artefacts
//
