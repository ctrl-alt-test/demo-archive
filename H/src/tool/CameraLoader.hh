#pragma once

#if DEBUG
# define CAMERA_EDIT 1
#else // !DEBUG
# define CAMERA_EDIT 0
#endif // !DEBUG

#if DEBUG

namespace Core
{
	struct CameraDescription;
}

namespace Timeline
{
	class Shot;
	class Timeline;
	struct IScene;
}

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, Timeline::IScene*> SceneDict;

void ParseCameraFile(const char* path, Timeline::Timeline& timeline, SceneDict& scenes);
void PrintCamera(const Core::CameraDescription& camera);
void PrintShot(Timeline::Shot& shot, const SceneDict& allScenes);

#endif
