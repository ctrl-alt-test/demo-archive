#ifndef MATRIX_HH
#define MATRIX_HH

namespace Algebra
{
	struct plane;
	struct quaternion;

	template<typename T> struct vector3;
	typedef vector3<float> vector3f;

	template<typename T> struct vector4;
	typedef vector4<float> vector4f;

	typedef vector3f point3f;

	/// <summary>
	/// 4x4 matrix.
	/// </summary>
	struct matrix4
	{
		union
		{
			// Warning: using column major notation.
			// | 0 4  8 12 |
			// | 1 5  9 13 |
			// | 2 6 10 14 |
			// | 3 7 11 15 |
			struct
			{
				float  _0,  _1,  _2,  _3; // X column
				float  _4,  _5,  _6,  _7; // Y column
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

		static const matrix4 identity;

		vector3f	xAxis() const;
		vector3f	yAxis() const;
		vector3f	zAxis() const;
		matrix4		transposed() const;
		matrix4		triviallyInverted() const;
		matrix4		inverted() const;

		static matrix4 basis(const vector3f& ux, const vector3f& uy, const vector3f& uz);
		static matrix4 orthonormalBasis(const vector3f& x, const vector3f& y, const vector3f& z);
		static matrix4 lookAt(const vector3f& pos, const vector3f& target, const vector3f& up);

		/// <summary>
		/// Create an orthographic projection matrix.
		/// </summary>
		static matrix4 ortho(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		/// Create a perpective projection matrix.
		/// Uses matrix4::frustum internally.
		/// </summary>
		static matrix4 perspective(float fov, float aspectRatio, float near, float far);

		/// <summary>
		/// Create a frustum matrix.
		/// </summary>
		static matrix4 frustum(float left, float right, float bottom, float top, float near, float far);

		static matrix4 rotation(float angle, vector3f axis);
		static matrix4 rotation(float angle, float x, float y, float z);
		static matrix4 translation(const vector3f& t);
		static matrix4 translation(float x, float y, float z);
		static matrix4 scaling(const vector3f& s);
		static matrix4 scaling(float x, float y, float z);
		static matrix4 scaling(float s);

		matrix4&	rotate(float angle, const vector3f& axis);
		matrix4&	rotate(float angle, float x, float y, float z);
		matrix4&	translate(const vector3f& t);
		matrix4&	translate(float x, float y, float z);
		matrix4&	scale(const vector3f& s);
		matrix4&	scale(float x, float y, float z);
		matrix4&	scale(float s);

		matrix4&	operator += (const matrix4& mat);
		matrix4&	operator -= (const matrix4& mat);
		matrix4&	operator *= (const matrix4& mat);
		matrix4&	operator *= (float a);
		matrix4&	operator /= (float a);

	private:
		void operator * (const vector3f& rhs);
	};

	bool		operator == (const matrix4& lhs, const matrix4& rhs);
	bool		operator != (const matrix4& lhs, const matrix4& rhs);

	matrix4		operator + (const matrix4& lhs, const matrix4& rhs);
	matrix4		operator - (const matrix4& lhs, const matrix4& rhs);
	matrix4		operator * (const matrix4& lhs, const matrix4& rhs);
	matrix4		operator * (const matrix4& lhs, float rhs);
	matrix4		operator / (const matrix4& lhs, float rhs);
	vector4f	operator * (const matrix4& mat, const vector4f& v);
	vector3f	operator * (const matrix4& mat, const vector3f& v);

	matrix4		computeMatrix(const vector3f& v, const quaternion& q);
	//matrix4 computeInvertedMatrix(const Transformation& t);

	void		transpose(matrix4& m);

	/// <summary>
	/// Invert the matrix assuming it is only a translation and a rotation.
	/// </summary>
	void		triviallyInvert(matrix4& m);

	/// <summary>
	/// Invert the matrix with no other assumption that it is invertible.
	/// </summary>
	void		invert(matrix4& m);

	/// <summary>
	/// Transforms a direction with a given matrix.
	/// v is assumed to be a direction, it WON'T get translated!
	/// <summary>
	void		applyMatrixToDirection(const matrix4& m, vector3f& v);

	/// <summary>
	/// Transforms a position with a given matrix.
	/// p is assumed to be a point, it WILL get translated!
	/// <summary>
	void		applyMatrixToPoint(const matrix4& m, point3f& p);
	void		applyMatrixToPlane(const matrix4& m, plane& s);
}

#endif // MATRIX_HH
