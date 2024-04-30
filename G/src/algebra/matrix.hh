//
// Matrice 4x4
//

#ifndef		MATRIX_HH
# define	MATRIX_HH

// FIXME: virer cet include d'ici
#include "textureusage.hh"

struct Transformation;

template<typename T> struct vector3;
typedef vector3<float> vector3f;

template<typename T> struct vector4;
typedef vector4<float> vector4f;

typedef vector3f point3f;

// | 0 4  8 12 |
// | 1 5  9 13 |
// | 2 6 10 14 |
// | 3 7 11 15 |
struct matrix4
{
  union
  {
    // Attention :
    // On utilise OpenGL, qui est "column-major", donc on utilise du
    // column major aussi. Affiché ligne par ligne comme ici, ça fait
    // le contraire de la notation habituelle utilisée à l'école.

    struct
    {
      float  _0,  _1,  _2,  _3; // Colone X
      float  _4,  _5,  _6,  _7; // Colone Y
      float  _8 , _9, _10, _11; // etc.
      float _12, _13, _14, _15;
    };
    struct
    {
      float m00, m10, m20, m30;
      float m01, m11, m21, m31;
      float m02, m12, m22, m32;
      float m03, m13, m23, m33;
    };
    float m[16];
  };

  static const matrix4	identity;

  vector3f	xAxis() const;
  vector3f	yAxis() const;
  vector3f	zAxis() const;
  matrix4	transpose() const;

  static matrix4 basis(const vector3f & ux, const vector3f & uy, const vector3f & uz);
  static matrix4 orthonormalBasis(const vector3f & x, const vector3f & y, const vector3f & z);
  static matrix4 lookAt(const vector3f & pos, const vector3f & target, const vector3f & up);
  static matrix4 rotation(const float angle, vector3f axis);
  static matrix4 translation(const vector3f & t);
  static matrix4 scaling(const vector3f & s);

  matrix4 &	rotate(const float angle, const vector3f & axis);
  matrix4 &	translate(const vector3f & t);
  matrix4 &	scale(const vector3f & s);

  matrix4 &	operator += (const matrix4 & mat);
  matrix4 &	operator -= (const matrix4 & mat);
  matrix4 &	operator *= (const matrix4 & mat);
  matrix4 &	operator *= (float a);
  matrix4 &	operator /= (float a);
};

matrix4	operator + (const matrix4 & lhs, const matrix4 & rhs);
matrix4	operator - (const matrix4 & lhs, const matrix4 & rhs);
matrix4	operator * (const matrix4 & lhs, const matrix4 & rhs);
matrix4	operator * (const matrix4 & lhs, float rhs);
matrix4	operator / (const matrix4 & lhs, float rhs);
vector4f operator * (const matrix4 & mat, const vector4f & v);

matrix4 computeMatrix(const Transformation & t);
matrix4 computeInvMatrix(const Transformation & t);

void invMatrix(matrix4 & a);

void getFromProjection(matrix4 & dst);
void getFromModelView(matrix4 & dst);
void setToModelView(const matrix4 & src);
void setToTextureMatrix(const matrix4 & src, Texture::usage u);

void applyMatrixToVector(const matrix4 & m, vector3f & v);
void applyMatrixToPoint(const matrix4 & m, point3f & v);

#endif // MATRIX_HH
