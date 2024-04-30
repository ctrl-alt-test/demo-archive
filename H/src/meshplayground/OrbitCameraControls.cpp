#include "OrbitCameraControls.hh"

#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Array.hxx"
#include "engine/core/Camera.hxx"
#include "tool/KeyboardAndMouseState.hh"
#include "tweakval/tweakval.h"

OrbitCameraControls::OrbitCameraControls():
	enableFaceCulling(true),
	wireFrame(false)
{
}

Core::RigidTransform OrbitCameraControls::GetTransform()
{
	return
		(Core::RigidTransform::translation(0, 0, -distance) *
		Core::RigidTransform::rotation(latitude, Algebra::vector3f::ux) *
		Core::RigidTransform::rotation(longitude, Algebra::vector3f::uy) *
		Core::RigidTransform::translation(anchor)).Inverse();
}

Core::Camera OrbitCameraControls::GetCamera()
{
	Core::CameraDescription cameraDesc = { GetTransform(), fov };
	return CreateCamera(cameraDesc, aspectRatio, _TV(1.f), _TV(400.f));
}

void OrbitCameraControls::OnMouseMove(const Tool::MouseState& mouseState,
									  const Tool::MouseState& mouseDiff,
									  const Tool::KeyboardState& keyboardState)
{
	int speed = 1;
	if (keyboardState.leftShiftKeyDown || keyboardState.rightShiftKeyDown)
	{
		speed = 0;
	}
	else if (keyboardState.leftCtrlKeyDown || keyboardState.rightCtrlKeyDown)
	{
		speed = 2;
	}
	const float rotationSpeeds[] = { 0.0025f, 0.005f, 0.01f, };
	const float translationSpeeds[] = { 0.00025f, 0.001f, 0.004f, };
	const float wheelTranslationSpeeds[] = { 0.001f, 0.005f, 0.025f, };

	if (mouseState.left)
	{
		longitude += rotationSpeeds[speed] * mouseDiff.x;
		latitude = msys_max(-90.f * DEG_TO_RAD, msys_min(90.f * DEG_TO_RAD, latitude + rotationSpeeds[speed] * mouseDiff.y));
	}
	if (mouseState.middle)
	{
		Algebra::vector3f translation = {
			mouseDiff.x * cosf(longitude) - mouseDiff.y * sinf(latitude) * sinf(longitude),
			-mouseDiff.y * cosf(latitude),
			mouseDiff.x * sinf(longitude) + mouseDiff.y * sinf(latitude) * cosf(longitude),
		};
		anchor += translationSpeeds[speed] * distance * translation;
	}
	if (mouseState.wheel != 0)
	{
		distance += wheelTranslationSpeeds[speed] * mouseState.wheel;
	}
}

void OrbitCameraControls::OnMouseButton(const Tool::MouseState& /*mouseState*/,
										const Tool::MouseState& /*mouseDiff*/,
										const Tool::KeyboardState& /*keyboardState*/)
{
}

void OrbitCameraControls::OnKeyboard(const platform::KeyEvent& event,
									 const Tool::KeyboardState& /*keyboardState*/)
{
	if (!event.pressed)
	{
		return;
	}

	switch (event.key)
	{
	case platform::KeyCode::keyNumpadMultiply:
		enableFaceCulling = !enableFaceCulling;
		break;
	case platform::KeyCode::keyTab:
	case platform::KeyCode::keyNumpadDivide:
		wireFrame = !wireFrame;
		break;
	default:
		break;
	};
}
