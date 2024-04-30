//
// Vecteur 3D
//

#ifndef		VECTOR3_HXX
# define	VECTOR3_HXX

#include "vector3.hh"

#include "quaternion.hh"

#include "sys/msys_libc.h"
#include "sys/msys_debug.h"

#define USE_NAIVE    1
#define USE_SSE      0
#define USE_TEMPLATE 0

#if USE_SSE
# include <smmintrin.h>
#endif

template<typename T> const vector3<T> vector3<T>::zero(0, 0, 0);
template<typename T> const vector3<T> vector3<T>::ux(1, 0, 0);
template<typename T> const vector3<T> vector3<T>::uy(0, 1, 0);
template<typename T> const vector3<T> vector3<T>::uz(0, 0, 1);

// ----------------------------------------------------------------------------
// Version C++ naïve

#if USE_NAIVE

//
// Opérateurs arithmétiques unaires
//
template<typename T>
inline
vector3<T> & vector3<T>::operator += (const vector3<T> & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

template<typename T>
inline
vector3<T> & vector3<T>::operator -= (const vector3<T> & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

template<typename T>
inline
vector3<T> & vector3<T>::operator *= (const T & a)
{
  x *= a;
  y *= a;
  z *= a;
  return *this;
}

template<typename T>
inline
vector3<T> & vector3<T>::operator /= (const T & a)
{
  *this *= (T)1 / a;
  return *this;
}

//
// Opérateurs de comparaison
//
template<typename T>
inline
bool operator == (const vector3<T> & lhs, const vector3<T> & rhs)
{
  return ((lhs.x == rhs.x) &&
	  (lhs.y == rhs.y) &&
	  (lhs.z == rhs.z));
}

template<typename T>
inline
bool operator != (const vector3<T> & lhs, const vector3<T> & rhs)
{
  return !(lhs == rhs);
}

//
// Opérateurs arithmétiques binaires
//
template<typename T>
inline
vector3<T> operator + (const vector3<T> & lhs, const vector3<T> & rhs)
{
  vector3<T> result = lhs;
  result += rhs;
  return result;
}

template<typename T>
inline
vector3<T> operator - (const vector3<T> & lhs, const vector3<T> & rhs)
{
  vector3<T> result = lhs;
  result -= rhs;
  return result;
}

template<typename T>
inline
vector3<T> operator * (const vector3<T> & lhs, const T & rhs)
{
  vector3<T> result = lhs;
  result *= rhs;
  return result;
}

template<typename T>
inline
vector3<T> operator / (const vector3<T> & lhs, const T & rhs)
{
  return lhs * ((T)1 / rhs);
}


template<typename T>
inline
vector3<T> operator * (const T & lhs, const vector3<T> & rhs)
{
  return rhs * lhs;
}

template<typename T>
inline
vector3<T> operator - (const vector3<T> & v)
{
  return vector3<T>(-v.x, -v.y, -v.z);
}


//
// Produit scalaire
//
template<typename T>
inline
T dot(const vector3<T> & u, const vector3<T> & v)
{
  return (u.x * v.x +
	  u.y * v.y +
	  u.z * v.z);
}

// produit vectoriel
template<typename T>
inline
vector3<T> cross(const vector3<T> & u, const vector3<T> & v)
{
  return vector3<T>(u.y * v.z - u.z * v.y,
		    u.z * v.x - u.x * v.z,
		    u.x * v.y - u.y * v.x);
}

//
// Norme
//
inline
float norm(const vector3f & v)
{
  return msys_sqrtf(dot(v, v));
}

inline
void normalize(vector3f & v)
{
  const float n = norm(v);
  assert(n != 0);
  v *= (1.f / n);
}

inline
vector3f mix(const vector3f & u, const vector3f & v, float x)
{
  return u + (v - u) * x;
}

inline
float angle(vector3f u, vector3f v)
{
  normalize(u);
  normalize(v);
  return msys_acosf(dot(u, v));
}

inline
vector3f rotate(const quaternion & q, const vector3f & v)
{
  const quaternion rv(q * quaternion(v.x, v.y, v.z, 0) * conjugate(q));
  return vector3f(rv.x, rv.y, rv.z);
}

#endif // USE_NAIVE

// ----------------------------------------------------------------------------
// Version SIMD

#if USE_SSE

inline
vector3f & vector3f::operator += (const vector3f & v)
{
  m128 = _mm_add_ps(m128, v.m128);
  return *this;
}

inline
vector3f & vector3f::operator -= (const vector3f & v)
{
  m128 = _mm_sub_ps(m128, v.m128);
  return *this;
}

inline
vector3f & vector3f::operator *= (float a)
{
  m128 = _mm_mul_ps(m128, _mm_set_ps1(a));
  return *this;
}

inline
vector3f & vector3f::operator /= (float a)
{
  m128 = _mm_div_ps(m128, _mm_set_ps1(a));
  return *this;
}


inline
vector3f operator + (const vector3f & lhs, const vector3f & rhs)
{
  return vector3f(_mm_add_ps(lhs.m128, rhs.m128));
}

inline
vector3f operator - (const vector3f & lhs, const vector3f & rhs)
{
  return vector3f(_mm_sub_ps(lhs.m128, rhs.m128));
}

inline
vector3f operator * (const vector3f & lhs, float rhs)
{
  return vector3f(_mm_mul_ps(lhs.m128, _mm_set_ps1(rhs)));
}

inline
vector3f operator / (const vector3f & lhs, float rhs)
{
  return vector3f(_mm_div_ps(lhs.m128, _mm_set_ps1(rhs)));
}


inline
vector3f operator - (const vector3f & v)
{
  return vector3f(_mm_sub_ps(_mm_set_ps1(0.f), v.m128));
}


inline
float dot(const vector3f & u, const vector3f & v)
{
  return _mm_cvtss_f32(_mm_dp_ps(u.m128, v.m128, 0x71));
}

inline
vector3f cross(const vector3f & u, const vector3f & v)
{
  return vector3f(_mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(u.m128, u.m128, _MM_SHUFFLE(3, 0, 2, 1)),
					_mm_shuffle_ps(v.m128, v.m128, _MM_SHUFFLE(3, 1, 0, 2))),
			     _mm_mul_ps(_mm_shuffle_ps(u.m128, u.m128, _MM_SHUFFLE(3, 1, 0, 2)),
					_mm_shuffle_ps(v.m128, v.m128, _MM_SHUFFLE(3, 0, 2, 1)))));
}

inline
float norm(const vector3f & v)
{
  return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v.m128, v.m128, 0x71)));
}

inline
vector3f normalize(vector3f & v)
{
  return _mm_mul_ps(v.m128, _mm_rsqrt_ps(_mm_dp_ps(v.m128, v.m128, 0x7F)));
}

inline
vector3f mix(const vector3f & u, const vector3f & v, float x)
{
  return u + (v - u) * x;
  return _mm_add_ps(u.m128, _mm_mul_ps(_mm_sub_ps(v.m128, u.m128), _mm_set_ps1(x)));
}

inline
float angle(const vector3f & u, const vector3f & v)
{
  vector3f a(u);
  vector3f b(v);
  normalize(a);
  normalize(b);
  return msys_acosf(dot(u, v));
}


#endif // USE_SSE


// ----------------------------------------------------------------------------
// Version templates

#if USE_TEMPLATE

//
// http://www.flipcode.com/archives/Faster_Vector_Math_Using_Templates.shtml
//

namespace vector
{
  //
  // Arguments
  //
  template<class ta_a>
  class vecarg
  {
    const ta_a & _argv;

  public:
    inline
    vecarg(const ta_a & a):
      _argv(a)
    {}

    inline
    const float evaluate(int i) const
    {
      return _argv.evaluate(i);
    }
  };

  template<>
  class vecarg<const float>
  {
    const ta_a& _argv;

  public:
    inline
    vecarg(const ta_a& a):
      _argv(a)
    {}

    inline
    const float evaluate(int i) const
    {
      return _argv;
    }
  };


  //
  // Expressions
  //
  template<class ta_a, class ta_b, class ta_eval>
  class vecexp_2
  {
    const vecarg<ta_a> _arg1;
    const vecarg<ta_b> _arg2;

  public:
    inline
    vecexp_2(const ta_a & a1, const ta_b & a2):
      _arg1(a1), _arg2(a2)
    {}

    inline
    const float evaluate(int i) const
    {
      return ta_eval::evaluate(i, arg1, arg2);
    }
  };

  template<class ta_a, class ta_eval>
  class vecexp_1
  {
    const vecarg<ta_a> _arg1;

  public:
    inline
    vecexp_1(const ta_a & a1):
      _arg1(a1)
    {}

    inline
    const float evaluate(int i) const
    {
      return ta_eval::evaluate(i, arg1.evaluate(i));
    }
  };


  //
  // Base class
  //
  template<int ta_dimension, class T>
  struct base : public T
  {
    inline
    const float evaluate(int i) const
    {
      return (*((T*)this))[i];
    }

    template<class ta>
    inline
    const vector3d & operator = (const ta & exp)
    {
      for (int i=0; i < ta_dimension; ++i)
      {
        (*((T*)this))[i] = exp.evaluate(i);
      }
    }
  }

  // Sum
  struct sum
  {
    template<class ta_a, class ta_b>
    inline static
    const float evaluate(int i, const ta_a & a, const ta_b & b)
    {
      return a.evaluate(i) + b.evaluate(i);
    }
  };

  template<class ta_c1, class ta_c2>
  inline
  const vecexp_2<const ta_c1, const ta_c2, sum> operator + ( const ta_c1 & pa, const ta_c2 & pb)
  {
    return vecexp_2<const ta_c1, const ta_c2, sum>(pa, pb);
  }

  //
  // Data
  //
  struct desc_xyz
  {
    float x;
    float y;
    float z;

    inline
    float & operator[](const int i) { return ((float*)this)[i]; }
  };
};

#endif // USE_TEMPLATE


#undef USE_NAIVE
#undef USE_SSE
#undef USE_TEMPLATE

#endif // VECTOR3_HXX
