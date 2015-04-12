uniform sampler2D nMap;

varying mat3 tanSpace;

vec2 vTexCoord;

//
// Normale perturbée par la normal map
//
vec3 getNormal()
{
  vec3 bump = texture2D(nMap, vTexCoord).xyz * 2. - 1.;
  return normalize(tanSpace * bump);
}
