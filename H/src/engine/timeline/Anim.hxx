#ifndef ANIM_HXX
#define ANIM_HXX

#include "Anim.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/container/Algorithm.hxx"

namespace Timeline
{
	template<typename T>
	Anim<T>::Anim():
		delegate(NULL), m_loops(1), m_dates(), m_frames(), m_interpolation(LinearInterpolation)
	{
	}

	template<typename T>
	Anim<T>::Anim(int numberOfFrames, unsigned int loops):
		delegate(NULL), m_dates(), m_frames(), m_interpolation(LinearInterpolation)
	{
		SetAnim(numberOfFrames, loops);
	}

	template<typename T>
	Anim<T>::Anim(int numberOfFrames, int* dates, T* frames):
		delegate(NULL), m_dates(), m_frames()
	{
		SetAnim(numberOfFrames, 1);
		int lastDate = 0;
		for (int i = 0; i < numberOfFrames; i++)
		{
			lastDate += dates[i];
			m_dates.add(lastDate);
			m_frames.add(frames[i]);
		}
		m_interpolation = numberOfFrames <= 2
			? LinearInterpolation
			: SplineInterpolation;
	}

	template<typename T>
	Anim<T>::Anim(const T& start, const T& end, int duration):
		delegate(NULL), m_loops(1), m_dates(), m_frames(), m_interpolation(LinearInterpolation)
	{
		SetAnim(start, end, duration);
	}

	template<typename T>
	Anim<T>::Anim(T (*delegate)(int)):
		delegate(delegate), m_interpolation(LinearInterpolation)
	{
	}

	template<typename T>
	void Anim<T>::SetAnim(int numberOfFrames, unsigned int loops)
	{
		assert(numberOfFrames >= 1);

		m_loops = loops;
		m_dates.init(numberOfFrames);
		m_frames.init(numberOfFrames);
	}

	template<typename T>
	void Anim<T>::SetAnim(const T& start, const T& end, int duration)
	{
		m_loops = 1;
		m_dates.init(2);
		m_frames.init(2);
		m_dates.add(0);
		m_dates.add(duration);
		m_frames.add(start);
		m_frames.add(end);
	}

	template<typename T>
	inline
	int Anim<T>::Duration() const
	{
		if (m_dates.size == 0)
		{
			return 0;
		}
		return m_dates.last();
	}

	template<typename T>
	inline
	void Anim<T>::Add(int date, const T& frame)
	{
		// The first frame must be at t=0, and frames must be added in
		// chronological order.
		assert((m_dates.size == 0 && 0 == date) || date >= Duration());

		m_dates.add(date);
		m_frames.add(frame);
	}

	template<typename T>
	T Anim<T>::Get(int localDate) const
	{
		// If it asserts here, make sure the animation is correctly
		// initialized.
		assert(delegate != NULL || Duration() >= 0);

		if (delegate != NULL)
		{
			return delegate(localDate);
		}

		if (Duration() == 0 || (m_loops != 0 && localDate / Duration() >= m_loops))
		{
			return m_frames.last();
		}

		const int seekDate = localDate % Duration();
		assert(seekDate <= Duration());

		int index = Container::binarySearch(m_dates, seekDate);
		if (index > 0 && m_dates[index] > seekDate)
		{
			--index;
		}

		return _eval(index, seekDate);
	}

	template<typename T>
	T Anim<T>::_eval(int index, int seekDate) const
	{
		if (m_interpolation == SplineInterpolation)
		{
			float weights[4];
			int indices[4];
			_computeSplineWeights(index, seekDate, indices, weights);
			return Algebra::interpolate(m_frames[indices[0]],
										m_frames[indices[1]],
										m_frames[indices[2]],
										m_frames[indices[3]],
										weights);
		}

		assert(m_interpolation == SmoothstepInterpolation || m_interpolation == LinearInterpolation);

		float weight = Algebra::interpolate(m_dates[index], m_dates[index + 1], seekDate);
		if (m_interpolation == SmoothstepInterpolation)
		{
			weight = Algebra::smoothStep(weight);
		}
		return Algebra::mix(m_frames[index], m_frames[index + 1], weight);
	}

	// This function has been extracted from _eval. Since it doesn't depend on template T, the
	// compiler should generate a single instance of the function, and make the binary smaller.
	template<typename T>
	void Anim<T>::_computeSplineWeights(int index, int seekDate, int* indices, float* weights) const
	{
		float dates[4];
		for (int i = index - 1; i <= index + 2; ++i)
		{
			int k = msys_min(msys_max(0, i), m_dates.size - 1);
			if (k == index - 1 && m_dates[k] == m_dates[k + 1])
			{
				++k;
			}
			if (k == index + 2 && m_dates[k] == m_dates[k - 1])
			{
				--k;
			}
			indices[i - (index - 1)] = k;
			dates[i - (index - 1)] = (float)m_dates[k];
		}
		Algebra::computeSplineWeights((float)seekDate, dates, weights);
	}

}

#endif // ANIM_HXX
