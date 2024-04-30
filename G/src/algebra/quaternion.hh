//
// Quaternion
//

#ifndef		QUATERNION_HH
# define	QUATERNION_HH

struct matrix4;

struct quaternion
{
  float x;
  float y;
  float z;
  float w;

  quaternion(float qx, float qy, float qz, float qw):
    x(qx), y(qy), z(qz), w(qw)
  {}

  quaternion() {}

  quaternion &	operator += (const quaternion & q2);
  quaternion &	operator -= (const quaternion & q2);
  quaternion &	operator *= (float a);
  quaternion &	operator /= (float a);
};

quaternion	operator + (const quaternion & lhs, const quaternion & rhs);
quaternion	operator - (const quaternion & lhs, const quaternion & rhs);
quaternion	operator * (const quaternion & lhs, const quaternion & rhs);
quaternion	operator * (const quaternion & lhs, float rhs);
quaternion	operator * (float lhs, const quaternion & rhs);
quaternion	operator / (const quaternion & lhs, float rhs);
quaternion	operator - (const quaternion & q);

quaternion	conjugate(const quaternion & q);
float		dot(const quaternion & a, const quaternion & b);

float		norm(const quaternion & q);
void		normalize(quaternion & q);
float		dist(const quaternion & q1, const quaternion & q2);

quaternion	lerp(const quaternion & qa, const quaternion & qb, float x);
quaternion	slerp(const quaternion & qa, const quaternion & qb, float x);

quaternion	computeQuaternion(const matrix4 & a);

#if DEBUG

struct compressedQuaternion
{
  char x;
  char y;
  char z;
  char w;

  quaternion q() const;
};

void			testCompressQuaternion(const quaternion & q);
compressedQuaternion	compressQuaternion(const quaternion & q, bool debug = false);

#endif // DEBUG


#endif // QUATERNION_HH
