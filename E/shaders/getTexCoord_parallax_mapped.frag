uniform sampler2D nMap;

varying vec3 vViewVecInv;

//
// Coordonnées de texture perturbées par le parallax mapping
//
vec2 getTexCoord()
{
  vec2 uv = gl_TexCoord[0].st / gl_TexCoord[0].q;

  float height = texture2D(nMap, uv).a;
  vec3 nViewVecInv = normalize(vViewVecInv);
  vec2 vOffset = 0.03 * height * nViewVecInv.xy;

  return uv + 0.25 * vOffset;
}
