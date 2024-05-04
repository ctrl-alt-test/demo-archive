#include "DebugControls.hh"

#include "KeyboardAndMouseState.hh"
#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"
#include "engine/core/Camera.hh"
#include "engine/core/Debug.hh"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/msys_temp.hh"
#include "engine/timeline/Anim.hxx"
#include "engine/timeline/Clock.hh"
#include "tool/CameraLoader.hh"

#ifdef ENABLE_PLAYER_64KLANG2
#include "64klang2_Player/MusicPlayer64k2.hh"
#endif // ENABLE_PLAYER_64KLANG2
#ifdef ENABLE_PLAYER_BASS
#include "engine/sound/MusicPlayerBASS.hh"
#endif // ENABLE_PLAYER_BASS

using namespace Tool;

MouseState mouseState = { 0, 0, 0, false, false, false, false, false, };
MouseState mousePreviousState = { 0, 0, 0, false, false, false, false, false, };
KeyboardState keyboardState = { false, false, false, false, false };
Container::Array<platform::KeyEvent> pendingKeyEvents(128);

DebugControls::DebugControls():
	showStats(true),
	enableBloom(true),
	enableLightStreak(true),
	stopAtDrawCall(0),
	enableFaceCulling(true),
	wireFrame(false),
	shadingMode(Standard),
	selectedObject(0), // Camera selected by default.
	selectedCamera(defaultCameraMode),
	manualCamera(),

	clock(NULL)
{
	manualCamera.transform.v = Algebra::vector3f::zero;
	manualCamera.transform.q = Algebra::quaternion::identity;
	manualCamera.fov = 90 * DEG_TO_RAD;

	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		useManualLight[i] = false;
		manualLight[i].v = Algebra::vector3f::zero;
		manualLight[i].q = Algebra::quaternion::identity;
	}

	editingShot.youtubeTime = 0;
	editingShot.storyTime = 0;
	editingShot.scene = NULL;
	editingShot.positions = new Timeline::Anim<Algebra::vector3f>(100);
	editingShot.orientation = new Timeline::Anim<Algebra::quaternion>(100);
	editingShot.fov = new Timeline::Anim<float>(100);
}

void DebugControls::Update(long currentTime)
{
	manualClock.Update(currentTime);
	const MouseState mouseDiff = MouseState::MouseDiff(mousePreviousState, mouseState);
	mousePreviousState = mouseState;
	if (mouseDiff.x != 0 || mouseDiff.y != 0 || mouseDiff.wheel != 0)
	{
		OnMouseMove(mouseState, mouseDiff, keyboardState);
	}
	if (mouseDiff.left || mouseDiff.right || mouseDiff.middle || mouseDiff.x1 || mouseDiff.x2)
	{
		OnMouseButton(mouseState, mouseDiff, keyboardState);
	}

	for (int i = 0; i < pendingKeyEvents.size; ++i)
	{
		const platform::KeyEvent& event = pendingKeyEvents[i];
		keyboardState.ReadEvent(event);
		OnKeyboard(event, keyboardState);
	}
	pendingKeyEvents.empty();
}

Core::RigidTransform* DebugControls::SelectedObject()
{
	switch (selectedObject)
	{
	case 0: return &manualCamera.transform;
	case 1: return &manualLight[0];
	case 2: return &manualLight[1];
	case 3: return &manualLight[2];
	case 4: return &manualLight[3];
	case 5: return &manualLight[4];
	default: return NULL;
	}
}

void DebugControls::PrintSelectedObjectInfo()
{
	Core::RigidTransform* transform = SelectedObject();
	if (transform != NULL)
	{
		// HACK: Assume 0 to be a camera.
		if (selectedObject == 0)
		{
			PrintCamera(manualCamera);
		}
		else
		{
			LOG_INFO("Light %d:", selectedObject - 1);
			LOG_RAW("{ { %ff, %ff, %ff, }, { %ff, %ff, %ff, %ff, }, },\n",
				transform->v.x, transform->v.y, transform->v.z,
				transform->q.x, transform->q.y, transform->q.z, transform->q.w);
		}
	}
	else
	{
		LOG_INFO("No object selected.");
	}
}

void DebugControls::NextCameraMode()
{
#if ENABLE_EDITING_CAMERA
	if (selectedCamera == defaultCameraMode)
		selectedCamera = manualCameraMode;
	else if (selectedCamera == editingCameraMode)
		selectedCamera = defaultCameraMode;
	else // manualCameraMode
	{
		if (editingShot.fov->Duration() > 0)
			selectedCamera = editingCameraMode;
		else
			selectedCamera = defaultCameraMode;
	}
#else // !ENABLE_EDITING_CAMERA
	if (selectedCamera == defaultCameraMode)
	{
		bool isPaused = clock->isPaused;
		clock->SetPaused(true);

		manualClock.SetPaused(isPaused);
		selectedCamera = manualCameraMode;
	}
	else
	{
		bool isPaused = manualClock.isPaused;
		manualClock.SetPaused(true);

		clock->SetPaused(isPaused);
		selectedCamera = defaultCameraMode;
	}
#endif // !ENABLE_EDITING_CAMERA
}

void DebugControls::ToggleUseOfSelectedObject()
{
	switch (selectedObject)
	{
	case 0: NextCameraMode(); break;
	case 1: useManualLight[0] = !useManualLight[0]; break;
	case 2: useManualLight[1] = !useManualLight[1]; break;
	case 3: useManualLight[2] = !useManualLight[2]; break;
	case 4: useManualLight[3] = !useManualLight[3]; break;
	case 5: useManualLight[4] = !useManualLight[4]; break;
	default:
		break;
	}
}

void TranslateObject(Core::RigidTransform* transform, float x, float y, float z)
{
	if (transform != NULL)
	{
		const Algebra::vector3f translation = { x, y, z };
		transform->translate(Algebra::rotate(transform->q, translation));
	}
}

void ResetObjectRotation(Core::RigidTransform* transform)
{
	if (transform != NULL)
	{
		transform->q = Algebra::quaternion::identity;
	}
}

void RotateObject(Core::RigidTransform* transform, float angle, float x, float y, float z)
{
	if (transform != NULL)
	{
		transform->rotate(angle, -x, -y, -z);
	}
}

void ChangeFoV(Core::CameraDescription* camera, float offset)
{
	float fov = camera->fov + offset * DEG_TO_RAD;
	fov = msys_max(msys_min(fov, 120.f * DEG_TO_RAD), 4.f * DEG_TO_RAD);
	camera->fov = fov;
}

void DebugControls::OnMouseButton(const Tool::MouseState& /*mouseState*/,
								  const Tool::MouseState& /*mouseDiff*/,
								  const Tool::KeyboardState& /*keyboardState*/)
{
}

void DebugControls::OnMouseMove(const Tool::MouseState& mouseState,
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
	const float rotationSpeeds[] = { 0.0005f, 0.003f, 0.006f, };
	const float translationSpeeds[] = { 0.002f, 0.01f, 0.1f, };
	const float wheelTranslationSpeeds[] = { 0.001f, 0.005f, 0.05f, };

	// HACK: Assume 0 to be a camera.
	float fovMultiplier = (selectedObject == 0 ? manualCamera.fov : 1.f);

	// Don't move the camera when it's not visible.
	if (selectedCamera == manualCameraMode ||
		selectedObject != 0)
	{
		if (mouseState.left)
		{
			RotateObject(SelectedObject(), fovMultiplier * rotationSpeeds[speed] * mouseDiff.x, 0, 1, 0);
			RotateObject(SelectedObject(), fovMultiplier * rotationSpeeds[speed] * mouseDiff.y, 1, 0, 0);
		}
		if (mouseState.middle)
		{
			TranslateObject(SelectedObject(), -translationSpeeds[speed] * mouseDiff.x, translationSpeeds[speed] * mouseDiff.y, 0);
		}
		if (mouseState.right)
		{
			RotateObject(SelectedObject(), rotationSpeeds[speed] * mouseDiff.x, 0, 0, 1);
		}
		if (mouseDiff.wheel != 0)
		{
			TranslateObject(SelectedObject(), 0, 0, wheelTranslationSpeeds[speed] * mouseDiff.wheel);
		}
	}
}

const Timeline::Clock* DebugControls::GetClock() const
{
	switch (selectedCamera)
	{
	case manualCameraMode:
#if ENABLE_EDITING_CAMERA
	case editingCameraMode:
#endif //ENABLE_EDITING_CAMERA
		return &manualClock;
	case defaultCameraMode:
		return clock;
	}
	assert(false);
	return clock;
}

Timeline::Clock* DebugControls::GetClock()
{
	switch (selectedCamera)
	{
	case manualCameraMode:
#if ENABLE_EDITING_CAMERA
	case editingCameraMode:
#endif //ENABLE_EDITING_CAMERA
		return &manualClock;
	case defaultCameraMode:
		return clock;
	}
	assert(false);
	return clock;
}

int DebugControls::GetStoryTime() const
{
	switch (selectedCamera)
	{
	case manualCameraMode:
#if ENABLE_EDITING_CAMERA
	case editingCameraMode:
#endif //ENABLE_EDITING_CAMERA
		return manualClock.YoutubeTime();
	case defaultCameraMode:
		return defaultShot->GetStoryTime(clock->YoutubeTime());
	}
	assert(false);
	return 0;
}

const Timeline::Shot* DebugControls::GetShot() const
{
	switch (selectedCamera)
	{
	case manualCameraMode:
#if ENABLE_EDITING_CAMERA
	case editingCameraMode:
#endif //ENABLE_EDITING_CAMERA
		return &editingShot;
	case defaultCameraMode:
		return defaultShot;
	}
	assert(false);
	return defaultShot;
}

Timeline::IScene* DebugControls::GetScene() const
{
	switch (selectedCamera)
	{
	case manualCameraMode:
#if ENABLE_EDITING_CAMERA
	case editingCameraMode:
#endif // ENABLE_EDITING_CAMERA
		if (editingShot.scene == NULL)
		{
			return defaultShot->scene;
		}
		return editingShot.scene;
	case defaultCameraMode:
		return defaultShot->scene;
	}
	assert(false);
	return NULL;
}

Core::CameraDescription DebugControls::GetCamera() const
{
	if (selectedCamera == manualCameraMode)
	{
		return manualCamera;
	}
#if ENABLE_EDITING_CAMERA
	else if (selectedCamera == editingCameraMode)
	{
		return editingShot.Get(manualClock.YoutubeTime() - editingShot.youtubeTime);
	}
#endif // ENABLE_EDITING_CAMERA
	else // defaultCameraMode
	{
		return defaultShot->Get(clock->YoutubeTime() - defaultShot->youtubeTime);
	}
}

void DebugControls::GetTimeSinceKeyFrame(long* fovKeyFrame, long* positionKeyFrame, long* orientationKeyFrame) const
{
	long time = GetClock()->YoutubeTime() - GetShot()->youtubeTime;

	*fovKeyFrame = 0;
	for (int i = 0; i < GetShot()->fov->GetDates().size; ++i)
	{
		if (GetShot()->fov->GetDates()[i] > time)
		{
			break;
		}
		*fovKeyFrame = time - GetShot()->fov->GetDates()[i];
	}

	*positionKeyFrame = 0;
	for (int i = 0; i < GetShot()->positions->GetDates().size; ++i)
	{
		if (GetShot()->positions->GetDates()[i] > time)
		{
			break;
		}
		*positionKeyFrame = time - GetShot()->positions->GetDates()[i];
	}

	*orientationKeyFrame = 0;
	for (int i = 0; i < GetShot()->orientation->GetDates().size; ++i)
	{
		if (GetShot()->orientation->GetDates()[i] > time)
		{
			break;
		}
		*orientationKeyFrame = time - GetShot()->orientation->GetDates()[i];
	}
}

void DebugControls::AddFrameToEditingShot(bool overrideLastFrame)
{
	int date;
	if (editingShot.fov->NumberOfFrames() == 0) // first frame
	{
		LOG_RAW("First frame added.\n");
		editingShot.scene = defaultShot->scene;
		editingShot.youtubeTime = manualClock.YoutubeTime();
		editingShot.storyTime = manualClock.YoutubeTime(),
		date = 0;
	}
	else
	{
		date = manualClock.YoutubeTime() - editingShot.youtubeTime;
		// if duration is incorrect, assume the move takes 1s.
		if (date <= editingShot.fov->Duration())
			date = editingShot.fov->Duration() + 1000;
	}

	if (overrideLastFrame)
	{
		if (editingShot.fov->NumberOfFrames() == 0)
		{
			LOG_RAW("No frame to edit.\n");
			return;
		}
		editingShot.fov->GetDates().last() = date;
		editingShot.fov->GetFrames().last() = manualCamera.fov;
		editingShot.positions->GetDates().last() = date;
		editingShot.positions->GetFrames().last() = manualCamera.transform.v;
		editingShot.orientation->GetDates().last() = date;
		editingShot.orientation->GetFrames().last() = manualCamera.transform.q;
	}
	else
	{
		editingShot.fov->Add(date, manualCamera.fov);
		editingShot.positions->Add(date, manualCamera.transform.v);
		editingShot.orientation->Add(date, manualCamera.transform.q);
	}

	if (date > 0)
	{
		PrintShot(editingShot, allScenes);
	}
}

void DebugControls::MusicPlayerSeek()
{
#if defined(ENABLE_PLAYER_64KLANG2) || defined(ENABLE_PLAYER_BASS)
	if (clock->isPaused)
	{
		player->Stop();
	}
	else
	{
		player->Play(clock->YoutubeTime());
	}
#endif // ENABLE_PLAYER_64KLANG2 || ENABLE_PLAYER_BASS
}

void DebugControls::OnKeyboard(const platform::KeyEvent& event,
							   const Tool::KeyboardState& keyboardState)
{
	if (!event.pressed)
	{
		switch (event.key)
		{
		case platform::KeyCode::key1:
		case platform::KeyCode::key2:
		case platform::KeyCode::key3:
		case platform::KeyCode::key4:
		case platform::KeyCode::key5:
		case platform::KeyCode::key7:
		case platform::KeyCode::key8:
		case platform::KeyCode::key9:
			// Maintain key to select object; 0 when key is released.
			selectedObject = 0;
			break;

			//
			// Weird: on Windows the print-screen key only emits a key
			// released event; no key pressed event is sent.
			//
		case platform::KeyCode::keyPrintScreen:
			PrintSelectedObjectInfo();
			break;

		default:
			break;
		}
		return;
	}

	int speed = 1;
	if (keyboardState.leftShiftKeyDown || keyboardState.rightShiftKeyDown)
	{
		speed = 0;
	}
	else if (keyboardState.leftCtrlKeyDown || keyboardState.rightCtrlKeyDown)
	{
		speed = 2;
	}
#ifdef ENABLE_PAUSE
	const int playerSpeeds[] = { 10, 1000, 10000, };
#endif // ENABLE_PAUSE
	const float translationSpeeds[] = { 0.1f, 1.f, 10.f, };
	const float fovSpeeds[] = { 0.1f, 1.f, 10.f, };

	switch (event.key)
	{
#ifdef ENABLE_PAUSE
		//
		// Demo "player" control
		//
	case platform::KeyCode::keySpace:
		if (GetClock() != NULL)
		{
			GetClock()->TogglePause();
			MusicPlayerSeek();
		}
		break;
	case platform::KeyCode::keyArrowLeft:
		if (GetClock() != NULL)
		{
			GetClock()->RelativeSeek(-playerSpeeds[speed]);
			MusicPlayerSeek();
		}
		break;
	case platform::KeyCode::keyArrowRight:
		if (GetClock() != NULL)
		{
			GetClock()->RelativeSeek(playerSpeeds[speed]);
			MusicPlayerSeek();
		}
		break;
#endif // ENABLE_PAUSE

		//
		// Moving objects around
		//
	case platform::KeyCode::keyW:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), 0, 0, -translationSpeeds[speed]);
		}
		break;
	case platform::KeyCode::keyA:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), -translationSpeeds[speed], 0, 0);
		}
		break;
	case platform::KeyCode::keyS:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), 0, 0, translationSpeeds[speed]);
		}
		break;
	case platform::KeyCode::keyD:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), translationSpeeds[speed], 0, 0);
		}
		break;
	case platform::KeyCode::keyE:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), 0, translationSpeeds[speed], 0);
		}
		break;
	case platform::KeyCode::keyC:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			TranslateObject(SelectedObject(), 0, -translationSpeeds[speed], 0);
		}
		break;
	case platform::KeyCode::keyR:
		if (selectedCamera == manualCameraMode || selectedObject != 0)
		{
			ResetObjectRotation(SelectedObject());
		}
		break;
	case platform::KeyCode::keyZ:
		if (selectedCamera == manualCameraMode && selectedObject == 0)
		{
			ChangeFoV(&manualCamera, fovSpeeds[speed]);
		}
		break;
	case platform::KeyCode::keyX:
		if (selectedCamera == manualCameraMode && selectedObject == 0)
		{
			ChangeFoV(&manualCamera, -fovSpeeds[speed]);
		}
		break;
	case platform::KeyCode::keyQ:
		ToggleUseOfSelectedObject();
		break;
	case platform::KeyCode::keyP:
	case platform::KeyCode::keyPrintScreen: // <-- Here for documentation only; this case won't happen.
		PrintSelectedObjectInfo();
		break;

		//
		// Select object
		//
	case platform::KeyCode::key1:
	case platform::KeyCode::key2:
	case platform::KeyCode::key3:
	case platform::KeyCode::key4:
	case platform::KeyCode::key5:
	case platform::KeyCode::key7:
	case platform::KeyCode::key8:
	case platform::KeyCode::key9:
		// Maintain key to select object; 0 when key is released.
		selectedObject = event.key - platform::KeyCode::key0;
		break;

		//
		// Frame debugger
		//
	case platform::KeyCode::keyPeriod:
	case platform::KeyCode::keyNumpadDecimal:
		showStats = !showStats;
		break;
	case platform::KeyCode::keyPlus:
	case platform::KeyCode::keyNumpadPlus:
		stopAtDrawCall += (keyboardState.leftCtrlKeyDown || keyboardState.rightCtrlKeyDown ? 10 : 1);
		break;
	case platform::KeyCode::keyMinus:
	case platform::KeyCode::keyNumpadMinus:
		stopAtDrawCall = msys_max(stopAtDrawCall - (keyboardState.leftCtrlKeyDown || keyboardState.rightCtrlKeyDown ? 10 : 1), 0);
		break;
	case platform::KeyCode::keyNumpadMultiply:
		enableFaceCulling = !enableFaceCulling;
		break;
	case platform::KeyCode::keyTab:
	case platform::KeyCode::keyNumpadDivide:
		wireFrame = !wireFrame;
		break;
	case platform::KeyCode::keyF11:
	case platform::KeyCode::keyNumpad0:
		shadingMode = Standard;
		break;
	case platform::KeyCode::keyF1:
	case platform::KeyCode::keyNumpad1:
		shadingMode = WhiteMaterial;
		break;
	case platform::KeyCode::keyF2:
	case platform::KeyCode::keyNumpad2:
		shadingMode = Highlight;
		break;
	case platform::KeyCode::keyF3:
	case platform::KeyCode::keyNumpad3:
		shadingMode = ShowTexCoord;
		break;
	case platform::KeyCode::keyNumpad7:
		enableBloom = !enableBloom;
		break;
	case platform::KeyCode::keyNumpad8:
		enableLightStreak = !enableLightStreak;
		break;

		//
		// Camera-specific keys
		//
	case platform::KeyCode::keyF:
		if (keyboardState.leftShiftKeyDown ||
			keyboardState.rightShiftKeyDown)
		{
			// Go to the begining of the shot.
			GetClock()->RelativeSeek(defaultShot->youtubeTime - clock->YoutubeTime());
		}
		else if (keyboardState.leftCtrlKeyDown ||
			keyboardState.rightCtrlKeyDown)
		{
			// Go to the end of the shot.
			GetClock()->RelativeSeek(defaultShot->youtubeTime + defaultShot->Duration() - clock->YoutubeTime());
		}
		editingShot.scene = defaultShot->scene;
		manualCamera = defaultShot->Get(clock->YoutubeTime() - defaultShot->youtubeTime);
		manualClock.AbsoluteSeek(defaultShot->GetStoryTime(clock->YoutubeTime()));
		break;
	case platform::KeyCode::keyInsert:
		AddFrameToEditingShot(keyboardState.leftCtrlKeyDown ||
			keyboardState.rightCtrlKeyDown);
		break;
	case platform::KeyCode::keyDelete:
		LOG_RAW("Editing camera has been deleted.\n");
#if ENABLE_EDITING_CAMERA
		if (selectedCamera == editingCameraMode)
		{
			selectedCamera = defaultCameraMode;
		}
#endif // ENABLE_EDITING_CAMERA
		editingShot.scene = NULL;
		editingShot.fov->Clear();
		editingShot.positions->Clear();
		editingShot.orientation->Clear();
		break;

	default:
		break;
	};
}

bool DebugControls::HandleKey(platform::KeyCode::Enum key, bool pressed)
{
	platform::KeyEvent event = { key, pressed };
	pendingKeyEvents.add(event);
	return false;
}

bool DebugControls::HandleMouse(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down)
{
	mouseState.x = x;
	mouseState.y = y;
	mouseState.wheel += wheel;
	mouseState.left = leftDown;
	mouseState.right = rightDown;
	mouseState.middle = middleDown;
	mouseState.x1 = x1Down;
	mouseState.x2 = x2Down;
	return false;
}
