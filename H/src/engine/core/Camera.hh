#ifndef CAMERA_HH
#define CAMERA_HH

#include "RigidTransform.hh"
#include "engine/algebra/Interpolation.hh"

namespace Core
{
	struct CameraDescription;

	struct CameraDescription
	{
		RigidTransform		transform;
		float				fov;
	};

	struct Camera
	{
		Algebra::matrix4	view;
		Algebra::matrix4	inverseView;
		Algebra::matrix4	projection;
		Algebra::matrix4	viewProjection;
	};

	Camera CreateCamera(const CameraDescription& description, float aspectRatio, float zNear, float zFar);
}

namespace Algebra
{
	template<>
	Core::CameraDescription mix(const Core::CameraDescription& start,
								const Core::CameraDescription& end,
								float x);

	template<>
	Core::CameraDescription interpolate(const Core::CameraDescription& camera1,
										const Core::CameraDescription& camera2,
										const Core::CameraDescription& camera3,
										const Core::CameraDescription& camera4,
										const float weights[4]);
}

#endif // CAMERA_HH
