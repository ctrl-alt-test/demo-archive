uniform sampler2D cMap;

vec2 vTexCoord;

//
// Albedo avec uniquement la première texture
//
vec4 getAlbedo()
{
  return texture2D(cMap, vTexCoord);
}
