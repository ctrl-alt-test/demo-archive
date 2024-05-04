#ifndef LIGHT_HH
#define LIGHT_HH

#include "engine/core/RigidTransform.hh"
#include "Camera.hh"

namespace Core
{
	struct Light
	{
		Core::RigidTransform	transform;
		Algebra::vector3f		color;
		float					coneAngle;

		// Temporary, until we have a better solution. Maybe a texture based one.
		int						type;

		Camera CreateLightCamera(float zNear, float zFar) const
		{
			Core::CameraDescription lightCameraDesc = { transform, coneAngle };
			return CreateCamera(lightCameraDesc, 1.f, zNear, zFar);
		}
	};
};

#endif // LIGHT_HH
