#version 120

uniform float zFar;
uniform float zNear;
uniform sampler2D tex2; // normal
uniform sampler2D tex5; // depth
uniform vec2 resolution;
uniform vec4 fogCol;

varying vec4 vBaryCoord;

//[
vec2 vTexCoord;
vec3 nViewVec;
//]

vec3 getHDRcolor(vec4 rgbe)
{
  return rgbe.rgb * pow(255., 1. - rgbe.a);
}

//
// Information du Z buffer, en linéaire
//
float getDepth(vec2 uv)
{
  float z = texture2D(tex5, uv).x;
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
  return normalize(texture2D(tex2, vTexCoord).xyz * 2. - 1.);
}

//
// Couleur représentative de la vitesse, pour la velocity map
//
// speed contient la direction xy en coordonnées homogènes
// La fonction renvoie la vitesse entre 0 et 1, avec la norme en z.
//
vec3 getSpeedColor(vec3 speed)
{
  vec2 s = speed.xy / speed.z;
  float norm = length(s);
  return vec3(0.5 + 0.5 * s / (0.001 + norm), norm);
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

  float height = texture2D(tex2, uv).a;
  vec2 vOffset = 0.03 * height * nViewVec.xy;

  return uv + 0.25 * vOffset;
}

//
// Couleur obtenue en tenant compte du fog
//
vec3 addFog(vec3 color, float intensity)
{
  float coef = max(0.001, 1. - fogCol.a);
  return mix(fogCol.rgb * pow(255., coef), color, intensity);
}

vec4 getEdges(float thickness, float alias, float flatness)
{
  float scale = resolution.x / 1024.;
  vec4 d = thickness * mix(vec4(0.04), scale * fwidth(vBaryCoord), clamp(flatness, 0., 1.));
  return 1. - smoothstep(d * (1. - alias), d * (1. + alias), vBaryCoord);
}

//
// Côté du polygone
//
// thickness: épaisseur des arrêtes
// alias : netteté
//   0 pour une coupure nette
//   1 pour autant de flou que l'épaisseur du trait
// flatness : épaisseur en fonction de la perspective
//   0 pour avoir la perspective
//   1 pour avoir une épaisseur constante
float getEdge(float thickness, float alias, float flatness)
{
  vec4 e = getEdges(thickness, alias, flatness);
  return max(max(e.x, e.y), max(e.z, e.w));
}

//
// Côté du polygone, en pointillés
//
/*
float getHatchedEdge(float thickness, float alias, float flatness, float dashes, float offset)
{
  vec4 edges = getEdges(thickness, alias, flatness);

  vec4 d = thickness * mix(vec4(0.04), 0.001 * resolution.x * fwidth(vBaryCoord), flatness);
  return fract(5.*d * vBaryCoord).x;
  //  vec4 hatch = clamp((2.* abs(fract(vBaryCoord * dashes + offset) - 0.5) - 0.4) / (alias * dashes), 0., 1.);
  //  edges *= hatch.yxyx;
  //  return max(max(edges.x, edges.y), max(edges.z, edges.w));
}
*/
