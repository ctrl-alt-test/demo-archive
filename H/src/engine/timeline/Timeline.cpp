#include "Timeline.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Quaternion.hxx"
#include "engine/core/Camera.hxx"
#include "engine/timeline/Anim.hxx"

const Timeline::Shot& Timeline::Timeline::GetShot(int youtubeTime)
{
	assert(m_shots.size > 0);

	int i = 0;
	int cutStartTime = 0;
	while (i + 1 < m_shots.size && youtubeTime >= cutStartTime + m_shots[i].Duration())
	{
		cutStartTime += m_shots[i++].Duration();
	}
	return m_shots[i];
}

Core::CameraDescription Timeline::Shot::Get(int time) const
{
	Core::RigidTransform transform = { positions->Get(time), orientation->Get(time) };
	Core::CameraDescription camera = { transform, fov->Get(time) };
	return camera;
}
