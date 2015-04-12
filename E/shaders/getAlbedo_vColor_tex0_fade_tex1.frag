uniform sampler2D cMap;
uniform sampler2D c2Map;
uniform float texFade;

varying vec4 vColor;

vec2 vTexCoord;

//
// Albedo avec la couleur des sommets modulée par un fade entre les
// deux textures
//
vec4 getAlbedo()
{
  return vColor * mix(texture2D(cMap, vTexCoord),
		      texture2D(c2Map, vTexCoord), texFade);
}
