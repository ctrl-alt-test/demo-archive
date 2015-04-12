uniform sampler2D cMap;

varying vec4 vColor;

vec2 vTexCoord;

//
// Albedo avec la première texture modulée par la couleur des sommets
//
vec4 getAlbedo()
{
  return vColor * texture2D(cMap, vTexCoord);
}
