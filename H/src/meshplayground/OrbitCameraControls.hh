#ifndef ORBITAL_CAMERA_HH
#define ORBITAL_CAMERA_HH

#include "engine/core/Camera.hh"
#include "platform/Platform.hh"

namespace Tool
{
	struct MouseState;
	struct KeyboardState;
}

struct OrbitCameraControls
{
	float	aspectRatio;
	float	fov;
	float	distance;
	float	latitude;
	float	longitude;
	Algebra::vector3f anchor;

	bool	enableFaceCulling;
	bool	wireFrame;

	OrbitCameraControls();
	Core::RigidTransform GetTransform();
	Core::Camera GetCamera();

	void OnKeyboard(const platform::KeyEvent& event,
					const Tool::KeyboardState& keyboardState);
	void OnMouseButton(const Tool::MouseState& mouseState,
					   const Tool::MouseState& mouseDiff,
					   const Tool::KeyboardState& keyboardState);
	void OnMouseMove(const Tool::MouseState& mouseState,
					 const Tool::MouseState& mouseDiff,
					 const Tool::KeyboardState& keyboardState);
};

#endif // ORBITAL_CAMERA_HH
