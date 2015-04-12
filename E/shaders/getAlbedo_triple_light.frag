// GLSL version 1.20 minimum, pour avoir Transpose()
#version 120

uniform sampler2D cMap;
uniform sampler2D dMap;
uniform vec3 trail1Color;
uniform vec3 trail2Color;
uniform vec3 trail3Color;

varying vec4 vColor;

vec2 vTexCoord;

//
// Albedo blanc avec
// la texture indiquant :
//  - R : motif lumineux 1
//  - G : motif lumineux 2
//  - B : motif lumineux 3
//  - A : ambient occlusion
//
// les 3 couleurs de motifs indiquent :
//  - RGB : la couleur
//  - A : l'intensité
//
// constante :
//  - couleur de base
//
vec4 getAlbedo()
{
  vec4 info = texture2D(cMap, vTexCoord);

  // Couleur du fond (blanc)
  vec3 shadow = vec3(0.729, 0.753, 0.804);
  vec3 c0 = mix(shadow, vec3(1.), pow(vColor.rgb, vec3(0.25))) * info.a;
  vec3 c1 = vec3(.9) * info.a;

  // Couleur dynamique
  vec3 cd = texture2D(dMap, gl_TexCoord[1].st).rgb;
  //  return vec4(cd, 1.);

  float footPrint = min(info.r + info.g + info.b, 1.);

  // Couleur des trois motifs
  mat3 cTrails = mat3(trail1Color, trail2Color, trail3Color);

  // Luminosité
  vec3 l = cd * info.rgb;
  float totalLightness = min(l.x + l.y + l.z, 1.);

  // Blending additif
  vec3 lcolor = cTrails * l;

  // Blending transparent
  vec3 color = mix(mix(c0, c1, footPrint) + 2. * lcolor, lcolor, totalLightness);

  return vec4(color, 1. - pow(max(.2 * footPrint, totalLightness), 2.));
}
