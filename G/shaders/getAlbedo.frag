#version 120

uniform sampler2D tex0;
// uniform sampler2D tex1;
uniform float texFade;

varying vec4 vColor;

//[
vec2 vTexCoord;
vec3 getHDRcolor(vec4 rgbe);
//]

//
// Albedo entièrement blanc
//
vec3 getAlbedo_white()
{
  return vec3(1.);
}

//
// Albedo avec uniquement la couleur des sommets
//
vec3 getAlbedo_vColor()
{
  return vColor.rgb;
}

//
// Albedo avec uniquement la première texture
//
vec3 getAlbedo_tex()
{
  vec4 c = texture2D(tex0, vTexCoord);
  return getHDRcolor(c);
}

//
// Albedo avec la première texture modulée par la couleur des sommets
//
vec3 getAlbedo_vColor_tex()
{
  vec4 c = vColor * texture2D(tex0, vTexCoord);
  return getHDRcolor(c);
}

//
// Albedo avec la couleur des sommets modulée par un fade entre les
// deux textures
//
 /*
vec3 getAlbedo_vColor_tex1_tex2_fade()
{
  vec4 c1 = texture2D(tex0, vTexCoord);
  vec4 c2 = texture2D(tex1, vTexCoord);
  return getHDRcolor(vColor * mix(c1, c2, texFade));
}
*/
