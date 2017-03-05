uniform sampler2D nMap;
uniform float zNear;
uniform float zFar;

//[
vec2 vTexCoord;
vec3 nViewVec;
//]

//
// Information du Z buffer, en linéaire
//
float getDepth(vec2 uv)
{
  float z = texture2D(nMap, uv).x;
  return (2.0 * zNear) / (zNear + mix(zFar, zNear, z));
}

//
// Normale interpolée suivant le modèle de Phong
//
vec3 getNormal_no_bump()
{
  return vec3(0., 0., 1.);
}

//
// Normale perturbée par la normal map
//
vec3 getNormal()
{
  return normalize(texture2D(nMap, vTexCoord).xyz * 2. - 1.);
}

//
// Couleur représentative de la vitesse, pour la velocity map
//
// speed contient la direction en xy, et une l'intensité en z
// (avec une correction type gamma pour avoir de la précision)
//
vec3 getSpeedColor(vec3 speed)
{
  float norm = length(speed.xy / speed.z);
  return vec3(0.5 + 0.5 * normalize(speed.xy / speed.z), pow(norm, 0.5));
}

//
// Coordonnées de texture de base
//
vec2 getTexCoord_no_bump()
{
  return gl_TexCoord[0].st / gl_TexCoord[0].q;
}

//
// Coordonnées de texture perturbées par le parallax mapping
//
vec2 getTexCoord()
{
  vec2 uv = gl_TexCoord[0].st / gl_TexCoord[0].q;

  float height = texture2D(nMap, uv).a;
  vec2 vOffset = 0.03 * height * nViewVec.xy;

  return uv + 0.25 * vOffset;
}

//
// Couleur obtenue en tenant compte du fog
//
vec4 addFog(vec4 color, float intensity)
{
  return vec4(mix(gl_Fog.color, color, intensity).xyz, color.a);
}
