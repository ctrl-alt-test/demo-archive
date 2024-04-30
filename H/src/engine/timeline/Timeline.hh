#ifndef TIMELINE_HH
#define TIMELINE_HH

#include "Anim.hh"
#include "engine/container/Array.hh"
#include "engine/core/Camera.hh"
#include "engine/core/msys_temp.hh"

namespace Gfx
{
	struct DrawArea;
}

namespace Timeline
{
	class Shot;

	/// <summary>
	/// A scene is a complete visual description, possibly animated and
	/// evolving over time. The camera is not part of the scene, but is
	/// passed as an argument during rendering.
	/// </summary>
	struct IScene
	{
		virtual void Draw(long currentStoryTime, const Core::CameraDescription& camera, const Gfx::DrawArea& drawArea) = 0;
	};

	/// <summary>
	/// A shot is an uninterrupted series of frames.
	///
	/// It can be implemented for example with keyframes, or as a
	/// function (procedural camera), or a combination of both (key
	/// frames and procedural camera shake).
	/// </summary>
	class Shot
	{
	public:
#if DEBUG
		char						shotName[81];
		char						sceneName[81];
#endif // DEBUG

		// The usual case is to use a shot once, and to make camera
		// with keyframes.
		//
		// Time when this shot will be played.
		int							youtubeTime;
		// Time in the world, when this shot was taken.
		int							storyTime;
		IScene*						scene;
		Anim<Algebra::vector3f>*	positions;
		Anim<Algebra::quaternion>*	orientation;
		Anim<float>*				fov;

		int GetStoryTime(int youtubeTime_) const
		{
			// We can add a coefficient here if we want time to go faster/slower.
			return storyTime + (youtubeTime_ - youtubeTime);
		}
		Core::CameraDescription Get(int time) const;

		int Duration() const
		{
			return msys_max(fov->Duration(), msys_max(positions->Duration(), orientation->Duration()));
		}
	};

	/// <summary>
	/// A timeline contains all the information to produce the final
	/// montage.
	///
	/// It exposes shots that indicate which camera and render list to
	/// use at any point in time.
	/// </summary>
	class Timeline
	{
	public:
		Timeline(): m_shots(512)
		{
		}

		const Shot&				GetShot(int youtubeTime);
		Container::Array<Shot>&	GetShots() { return m_shots; }

	public:
		// FIXME: propose a static implementation in release.
		Container::Array<Shot>	m_shots;
	};
}

#endif // TIMELINE_HH
