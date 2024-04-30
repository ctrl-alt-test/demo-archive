#ifndef VARIABLE_HH
#define VARIABLE_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/algebra/Vector4.hh"

// VAR_EDIT indicates whether variables can be modified at runtime
#if DEBUG
# define VAR_EDIT 1
#else // !DEBUG
# define VAR_EDIT 0 // no edit in release
#endif // !DEBUG

namespace Timeline
{
	enum InterpolationMode {
		modeLinear,
		modeSmoothstep,
		modeSpline,
		modeStep,
	};

	/// <summary>
	/// Container for a n-dimensional value evolving with time.
	/// </summary>
	class Variable
	{
	public:
		Variable();
		Variable(const float* data, int dim, int size, InterpolationMode mode);

		const float* data() const { return _data; }
		int		dim() const { return _dim; }
		int		size() const { return _size; }
		InterpolationMode	interpolation() const { return _interpolation; }


		void				get(int dim, float* out) const;
		float				get1() const;
		Algebra::vector2f	get2() const;
		Algebra::vector3f	get3() const;
		Algebra::vector4f	get4() const;

		void				get(float time, int dim, float* out) const;
		float				get1(float time) const;
		Algebra::vector2f	get2(float time) const;
		Algebra::vector3f	get3(float time) const;
		Algebra::vector4f	get4(float time) const;

		Algebra::vector3f	getLightColor(float time) const;

		void	update(float* data, int dim, int size, InterpolationMode mode);
		float	maxTime() const;
		void	setInterpolation(InterpolationMode mode) { _interpolation = mode; }

	private:
#if VAR_EDIT
		float*			_data;
#else // !VAR_EDIT
		const float*	_data;
#endif // !VAR_EDIT
		int				_dim;
		int				_size;
		InterpolationMode _interpolation;
	};
}

#endif // VARIABLE_HH
