#ifndef ANIM_HH
#define ANIM_HH

#include "engine/container/Array.hh"

namespace Timeline
{
	enum Interpolation
	{
		LinearInterpolation,
		SmoothstepInterpolation,
		SplineInterpolation,
	};

	template<typename T>
	/// <summary>
	/// Anim defines an animation of a T (float, vector, quaternion,
	/// etc.).
	///
	/// The following functions must be defined:
	/// T Algebra::mix(T, T, float)
	/// T Algebra::interpolate(T, T, T, T, float[4])
	/// </summary>
	class Anim
	{
	public:
		Anim();

		/// <summary>
		/// Creates an animation. Frames are added using .Add().
		/// </summary>
		Anim(int numberOfFrames, unsigned int loops = 1);

		/// <summary>
		/// Creates an animation that uses a static array.
		/// </summary>
		Anim(int numberOfFrames, int* dates, T* frames);

		/// <summary>
		/// Creates a simple linear animation between start and end.
		/// </summary>
		Anim(const T& start, const T& end, int duration);

		/// <summary>
		/// Creates a procedural animation using a delegate.
		/// </summary>
		explicit Anim(T (*delegate)(int));
		~Anim() {};

		void	SetAnim(int numberOfFrames, unsigned int loops = 1);
		void	SetAnim(const T& start, const T& end, int duration);
		int		Duration() const;
		int		NumberOfFrames() const { return m_dates.size; }
		void	SetInterpolation(Interpolation interpolation) { m_interpolation = interpolation; }

		void	Clear()
		{
			m_dates.empty();
			m_frames.empty();
		}

		Container::Array<int>& GetDates() { return m_dates; }
		Container::Array<T>& GetFrames() { return m_frames; }

		/// <summary>
		/// Assuming the animation is based on an array, adds a frame to the array.
		/// The first frame must be at t=0, and frames must be added in chronological order.
		/// </summary>
		void	Add(int date, const T& frame);
		T		Get(int localDate) const;

		T		(*delegate)(int localDate);

	private:
		T		_eval(int index, int seekDate) const;
		void	_computeSplineWeights(int index, int seekDate, int* indices, float* weights) const;

		int						m_loops; // 0 => infinity
		Container::Array<int>	m_dates;
		Container::Array<T>		m_frames;
		Interpolation			m_interpolation;
	};
}

#endif // ANIM_HH
