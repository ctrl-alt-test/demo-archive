//
// Vecteur
//

#include "sys/msys.h"
#include "sys/msys_debug.h"

#include "vector.hh"

//
// Attention, on considère que v subit la translation !
//
void computeMatrixPointProduct(const float m[16], vector3f & v)
{
  const float x = v.x * m[0] + v.y * m[4] + v.z * m[8]  + m[12];
  const float y = v.x * m[1] + v.y * m[5] + v.z * m[9]  + m[13];
  const float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
  v.x = x;
  v.y = y;
  v.z = z;
}

//
// Attention, on considère que v ne subit pas la translation !
//
void computeMatrixVectorProduct(const float m[16], vector3f & v)
{
  const float x = v.x * m[0] + v.y * m[4] + v.z * m[8];
  const float y = v.x * m[1] + v.y * m[5] + v.z * m[9];
  const float z = v.x * m[2] + v.y * m[6] + v.z * m[10];
  v.x = x;
  v.y = y;
  v.z = z;
}
