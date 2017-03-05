uniform sampler2D cMap;
uniform sampler2D dMap;
uniform float texFade;

//varying vec4 vColor;

vec2 vTexCoord;

//
// Albedo entièrement blanc
//
vec4 getAlbedo_white()
{
  return vec4(1.);
}

//
// Albedo avec uniquement la couleur des sommets
//
/*
vec4 getAlbedo_vColor()
{
  return vColor;
}
*/

//
// Albedo avec uniquement la première texture
//
vec4 getAlbedo_tex()
{
  return texture2D(cMap, vTexCoord);
}

//
// Albedo avec la première texture modulée par la couleur des sommets
//
/*
vec4 getAlbedo_vColor_tex()
{
  return vColor * texture2D(cMap, vTexCoord);
}
*/

//
// Albedo avec la couleur des sommets modulée par un fade entre les
// deux textures
//
 /*
vec4 getAlbedo_vColor_tex1_tex2_fade()
{
  return vColor * mix(texture2D(cMap, vTexCoord),
		      texture2D(dMap, vTexCoord), texFade);
}
*/
