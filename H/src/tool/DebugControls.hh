#ifndef DEBUG_CONTROLS_HH
#define DEBUG_CONTROLS_HH

#if DEBUG

#define ENABLE_EDITING_CAMERA 0

#include "engine/core/Camera.hh"
#include "engine/timeline/Clock.hh"
#include "engine/timeline/Timeline.hh"
#include "platform/KeyCodes.hh"
#include "tool/CameraLoader.hh"
#include "tool/ShaderHelper.hh"

namespace Sound
{
	class MusicPlayer64k2;
	class MusicPlayerBASS;
}

namespace Tool
{
	struct MouseState;
	struct KeyboardState;

	enum CameraMode
	{
		defaultCameraMode,
		manualCameraMode,
#if ENABLE_EDITING_CAMERA
		editingCameraMode,
#endif // ENABLE_EDITING_CAMERA
	};

	struct DebugControls
	{
		bool	showStats;
		bool	enableBloom;
		bool	enableLightStreak;
		int		stopAtDrawCall;
		bool	enableFaceCulling;
		bool	wireFrame;

		enum ShadingMode {
			Standard,
			Highlight,
			WhiteMaterial,
			ShowTexCoord,
		};
		ShadingMode	shadingMode;

		int						selectedObject;
		CameraMode				selectedCamera;
		Core::CameraDescription	manualCamera;
		bool					useManualLight[NUM_LIGHTS];
		Core::RigidTransform	manualLight[NUM_LIGHTS];

		SceneDict				allScenes;
		const Timeline::Shot*	defaultShot;
		Timeline::Shot			editingShot;
		Timeline::Clock*		clock;
		Timeline::Clock			manualClock;

#if defined(ENABLE_PLAYER_64KLANG2)
		Sound::MusicPlayer64k2*	player;
#elif defined(ENABLE_PLAYER_BASS)
		Sound::MusicPlayerBASS*	player;
#endif // ENABLE_PLAYER_BASS

		DebugControls();

		void Update(long currentRealTime);

		Core::RigidTransform*	SelectedObject();
		void PrintSelectedObjectInfo();
		void ToggleUseOfSelectedObject();

		const Timeline::Clock* GetClock() const;
		Timeline::Clock* GetClock(); // Yay C++
		int GetStoryTime() const;
		const Timeline::Shot* GetShot() const;
		Timeline::IScene* GetScene() const;
		Core::CameraDescription GetCamera() const;

		void GetTimeSinceKeyFrame(long* fovKeyFrame, long* positionKeyFrame, long* orientationKeyFrame) const;

		void NextCameraMode();
		void AddFrameToEditingShot(bool overrideLastFrame);
		void MusicPlayerSeek();

		void OnKeyboard(const platform::KeyEvent& event,
						const KeyboardState& keyboardState);
		void OnMouseButton(const MouseState& mouseState,
						   const MouseState& mouseDiff,
						   const KeyboardState& keyboardState);
		void OnMouseMove(const MouseState& mouseState,
						 const MouseState& mouseDiff,
						 const KeyboardState& keyboardState);

		static bool HandleKey(platform::KeyCode::Enum key, bool pressed);
		static bool HandleMouse(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down);
	};
}

#else // !DEBUG

namespace Tool
{
	struct DebugControls;
}

#endif // !DEBUG

#endif // DEBUG_CONTROLS_HH
