//
// Vecteur 3D
//

#ifndef		VECTOR3_HH
# define	VECTOR3_HH

#define USE_NAIVE    1
#define USE_SSE      0
#define USE_TEMPLATE 0

#if USE_SSE
# include <smmintrin.h>
#endif

struct quaternion;

// ----------------------------------------------------------------------------
// Version C++ naïve

#if USE_NAIVE

template<typename T>
struct vector3
{
  T x;
  T y;
  T z;

  vector3(const T & vx, const T & vy, const T & vz):
    x(vx),
    y(vy),
    z(vz)
  {}

  vector3(const T* v):
    x(v[0]),
    y(v[1]),
    z(v[2])
  {}

  vector3() {}

  static const vector3 zero;
  static const vector3 ux;
  static const vector3 uy;
  static const vector3 uz;

  vector3<T> &	operator += (const vector3<T> & v);
  vector3<T> &	operator -= (const vector3<T> & v);
  vector3<T> &	operator *= (const T & a);
  vector3<T> &	operator /= (const T & a);
};

typedef vector3<unsigned char>	vector3ub;
typedef vector3<int>		vector3i;
typedef vector3<float>		vector3f;

typedef vector3ub		point3ub;
typedef vector3i		point3i;
typedef vector3f		point3f;


template<typename T> bool	operator == (const vector3<T> & lhs, const vector3<T> & rhs);
template<typename T> bool	operator != (const vector3<T> & lhs, const vector3<T> & rhs);

template<typename T> vector3<T>	operator + (const vector3<T> & lhs, const vector3<T> & rhs);
template<typename T> vector3<T>	operator - (const vector3<T> & lhs, const vector3<T> & rhs);
template<typename T> vector3<T>	operator * (const vector3<T> & lhs, const T & rhs);
template<typename T> vector3<T>	operator / (const vector3<T> & lhs, const T & rhs);

template<typename T> vector3<T>	operator * (const T & lhs, const vector3<T> & rhs);
template<typename T> vector3<T>	operator - (const vector3<T> & v);

template<typename T> T		dot(const vector3<T> & u, const vector3<T> & v);
template<typename T> vector3<T>	cross(const vector3<T> & u, const vector3<T> & v);

float		norm(const vector3f & v);
void		normalize(vector3f & v);
vector3f	mix(const vector3f & u, const vector3f & v, float x);
float		angle(vector3f u, vector3f v);
vector3f	rotate(const quaternion & q, const vector3f & v);

#endif // USE_NAIVE


// ----------------------------------------------------------------------------
// Version SIMD

#if USE_SSE

__declspec(align(16))
struct vector3f
{
  union
  {
    struct
    {
	float x;
	float y;
	float z;
    };
    __m128 m128;
  };

  vector3f() {}

  vector3f(float vx, float vy, float vz):
      m128(_mm_set_ps(vx, vy, vz, 0.f))
    {}

  vector3f(__m128 v):
      m128(v)
    {}

  vector3f &	 operator += (const vector3f & v);
  vector3f &	 operator -= (const vector3f & v);
  vector3f &	 operator *= (float a);
  vector3f &	 operator /= (float a);
};

typedef vector3f point3f;


vector3f	operator + (const vector3f & lhs, const vector3f & rhs);
vector3f	operator - (const vector3f & lhs, const vector3f & rhs);
vector3f	operator * (const vector3f & lhs, float rhs);
vector3f	operator / (const vector3f & lhs, float rhs);

vector3f	operator * (float lhs, const vector3f & rhs);
vector3f	operator - (const vector3f & v);

float		dot(const vector3f & u, const vector3f & v);
vector3f	cross(const vector3f & u, const vector3f & v);

float		norm(const vector3f & v);
vector3f	normalize(vector3f & v);
vector3f	mix(const vector3f & u, const vector3f & v, float x);
float		angle(const vector3f & u, const vector3f & v);

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

#endif // VECTOR3_HH
