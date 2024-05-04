#include "Camera.hxx"

#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Interpolation.hxx"

using namespace Core;

template<>
CameraDescription Algebra::mix(const CameraDescription& start,
							   const CameraDescription& end,
							   float x)
{
	const CameraDescription result = {
		Algebra::mix(start.transform, end.transform, x),
		Algebra::mix(start.fov, end.fov, x),
	};
	return result;
}

template<>
CameraDescription Algebra::interpolate(const CameraDescription& camera1,
									   const CameraDescription& camera2,
									   const CameraDescription& camera3,
									   const CameraDescription& camera4,
									   const float weights[4])
{
	RigidTransform transform = Algebra::interpolate(camera1.transform,
													camera2.transform,
													camera3.transform,
													camera4.transform,
													weights);
	float fov = (camera1.fov * weights[0] +
				 camera2.fov * weights[1] +
				 camera3.fov * weights[2] +
				 camera4.fov * weights[3]);
	CameraDescription result = { transform, fov };
	return result;
}

Camera Core::CreateCamera(const CameraDescription& description, float aspectRatio, float zNear, float zFar)
{
	const Algebra::matrix4 inverseView = Core::ComputeMatrix(description.transform);
	const Algebra::matrix4 view = inverseView.inverted();
	const Algebra::matrix4 projection = Algebra::matrix4::perspective(description.fov, aspectRatio, zNear, zFar);
	const Algebra::matrix4 viewProjection = projection * view;

	const Camera result = {
		view,
		inverseView,
		projection,
		viewProjection,
	};
	return result;
}
