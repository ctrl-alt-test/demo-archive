attribute vec3 vTan;

//
// Matrice TBN pour passer entre le repère global et le repère tangeant
//
mat3 computeTBN()
{
  vec3 vNormal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 vTangent = normalize(gl_NormalMatrix * vTan);
  vec3 vBinormal = cross(vNormal, vTangent);

  return mat3(vTangent, vBinormal, vNormal);
}
