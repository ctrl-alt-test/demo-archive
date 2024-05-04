#include "LoadingBar.hh"

#include "engine/core/Debug.hh"
#include "engine/core/Settings.hh"
#include "engine/runtime/FileWatcher.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/timeline/Anim.hxx"
#include "engine/timeline/Clock.hh"
#include "gfx/OpenGL/OpenGLLayer.hh"
#include "imgui/imgui.h"
#include "platform/Platform.hh"
#include "tool/DebugGraphicLayer.hh"
#include "tool/ImGuiHelper.hh"
#include "tool/Server.hh"
#include "64klang2_Player/MusicPlayer64k2.hh"
#include "tweakval/tweakval.h"

#ifdef ENABLE_LAUNCH_DIALOG
#ifdef _WIN32
#include "platform/Win32Dialog.hh"
#endif // _WIN32
#endif // ENABLE_LAUNCH_DIALOG

#include "H/H.hh"

#if DEBUG
#include <iomanip>
#include <iostream>
#include <sstream>
#endif

#if DEBUG
#define DEBUG_FRAME 1 // 1 to be able to debug frame
#else // !DEBUG
#define DEBUG_FRAME 0
#endif // !DEBUG

#define DEMO_DURATION	235000

#if DEBUG
void GetReadableTime(int milliseconds, int& out_minutes, int& out_seconds, int& out_milliseconds)
{
	out_milliseconds = milliseconds % 1000;
	out_seconds = (milliseconds / 1000) % 60;
	out_minutes = (milliseconds / 60000);
}

template<typename T>
struct CircularBuffer
{
	Container::Array<T> buffer;
	int headIndex;

	CircularBuffer(int size):
		buffer(size),
		headIndex(-1)
	{
	}

	T& getNew()
	{
		headIndex = (headIndex + 1) % buffer.max_size;
		T& result = (buffer.size < buffer.max_size ? buffer.getNew() : buffer[headIndex]);

		return result;
	}
};
struct FrameSample
{
	long date;
	int drawCalls;
};
static CircularBuffer<FrameSample> frames(256);

void AddFrameDurationSample(const Timeline::Clock& clock, int drawCalls)
{
	FrameSample& frame = frames.getNew();
	frame.date = clock.realCurrentTime;
	frame.drawCalls = drawCalls;
}

float ComputeAverageFrameDuration(int numberOfFrames)
{
	if (frames.headIndex < 0)
	{
		return 0;
	}

	int tailIndex = frames.headIndex - numberOfFrames;
	if (tailIndex < 0)
	{
		if (frames.buffer.size < frames.buffer.max_size)
		{
			tailIndex = 0;
		}
		while (tailIndex < 0)
		{
			tailIndex += frames.buffer.max_size;
		}
	}

	long cumulativeDuration = frames.buffer[frames.headIndex].date - frames.buffer[tailIndex].date;
	assert(cumulativeDuration >= 0);

	int actualNumberOfFrames = frames.headIndex - tailIndex;
	if (actualNumberOfFrames < 0)
	{
		actualNumberOfFrames += frames.buffer.max_size;
	}

	return (actualNumberOfFrames != 0 ? float(cumulativeDuration) / float(actualNumberOfFrames): 0);
}

int ComputeMaxFrameDuration()
{
	int maxDuration = 1;
	for (int i = 1; i < frames.buffer.size; ++i)
	{
		int dt = frames.buffer[i].date - frames.buffer[i - 1].date;
		if (maxDuration < dt)
		{
			maxDuration = dt;
		}
	}

	return maxDuration;
}

int ComputeMaxFrameDrawCalls()
{
	int maxDrawCalls = 1;
	for (int i = 0; i < frames.buffer.size; ++i)
	{
		if (maxDrawCalls < frames.buffer[i].drawCalls)
		{
			maxDrawCalls = frames.buffer[i].drawCalls;
		}
	}

	return maxDrawCalls;
}

// FIXME: Surely there must be a better abstration for the music
// player.
void MusicPlayerSeek(const Timeline::Clock& clock, void* pPlayer)
{
#if defined(ENABLE_PLAYER_64KLANG2) || defined(ENABLE_PLAYER_BASS)
#if defined(ENABLE_PLAYER_64KLANG2)
	Sound::MusicPlayer64k2& player = *(Sound::MusicPlayer64k2*)pPlayer;
#elif defined(ENABLE_PLAYER_BASS)
	Sound::MusicPlayerBASS& player = *(Sound::MusicPlayerBASS*)pPlayer
#endif // ENABLE_PLAYER_BASS

	if (clock.isPaused)
	{
		player.Stop();
	}
	else
	{
		player.Play(clock.YoutubeTime());
	}
#endif // ENABLE_PLAYER_64KLANG2 || ENABLE_PLAYER_BASS
}

void ShowDebugWindow(float averageFrameDuration,
					 int drawCallsCount,
					 Timeline::Clock& clock,
					 const Tool::DebugControls& controls,
					 void* pPlayer)
{
	static bool showDebugWindow = true;
	static bool showImGuiDemo = false;
	int seekToDate = -1;

	if (showDebugWindow && showImGuiDemo)
	{
		// Quick test/demonstration of Dear ImGui.
		ImGui::ShowTestWindow();
	}

	if (showDebugWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(700, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("Testing ImGui for debug and tooling");

		if (ImGui::Button(showImGuiDemo ? "Hide ImGui features demo" : "Show ImGui features demo"))
		{
			showImGuiDemo = !showImGuiDemo;
		}

		if (ImGui::CollapsingHeader("Timeline"))
		{
			int minutes, seconds, milliseconds;

			const long youtubeTime = clock.YoutubeTime();
			GetReadableTime(youtubeTime, minutes, seconds, milliseconds);

			char shortYoutubeTime[8];
			sprintf(shortYoutubeTime, "%d'%02d\"", minutes, seconds);

			if (ImGui::Button(clock.isPaused ? "Play" : "Pause", ImVec2(120, 60)))
			{
				clock.TogglePause();
				MusicPlayerSeek(clock, pPlayer);
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::ProgressBar(float(youtubeTime) / float(DEMO_DURATION), ImVec2(0.0f,0.0f), shortYoutubeTime);

			int jumpToNumber = youtubeTime;
			if (ImGui::SliderInt("Seek", &jumpToNumber, 0, DEMO_DURATION, "%.0f ms"))
			{
				seekToDate = jumpToNumber;
			}
			if (ImGui::DragInt("DJ", &jumpToNumber, 10.f, 0, DEMO_DURATION, "%.0f ms"))
			{
				seekToDate = jumpToNumber;
			}
			ImGui::EndGroup();

			if (ImGui::TreeNode("Detail"))
			{
				ImGui::BeginGroup();
				ImGui::Text("Youtube time: %d'%02d\"%003d - %d ms", minutes, seconds, milliseconds, youtubeTime);

				// FIXME: This should be available directly in the clock.
				const long storyTime = controls.GetStoryTime();
				GetReadableTime(storyTime, minutes, seconds, milliseconds);
				ImGui::Text("Scene time:   %d'%02d\"%003d - %d ms", minutes, seconds, milliseconds, storyTime);

				GetReadableTime(clock.realCurrentTime, minutes, seconds, milliseconds);
				ImGui::Text("Real time:    %d'%02d\"%003d - %d ms", minutes, seconds, milliseconds, clock.realCurrentTime);
				ImGui::EndGroup();

				ImGui::SameLine();
				ImGui::Text("Time since last keyframe:");

				ImGui::SameLine();
				ImGui::BeginGroup();
				long fovKeyFrame;
				long positionKeyFrame;
				long orientationKeyFrame;
				controls.GetTimeSinceKeyFrame(&fovKeyFrame, &positionKeyFrame, &orientationKeyFrame);
				ImGui::Text("FoV:         %d ms", fovKeyFrame);
				ImGui::Text("Position:    %d ms", positionKeyFrame);
				ImGui::Text("Orientation: %d ms", orientationKeyFrame);
				ImGui::EndGroup();

				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Rendering"))
		{
			struct Local
			{
				static float FrameDurationPlot(void* data, int i)
				{
					const CircularBuffer<FrameSample>& frames = *(CircularBuffer<FrameSample>*)data;
					int index0 = frames.headIndex - (100 - i);
					while (index0 < 0 && frames.buffer.size == frames.buffer.max_size)
					{
						index0 += frames.buffer.max_size;
					}
					if (index0 < 0)
					{
						return 0;
					}
					int index1 = (index0 + 1) % frames.buffer.max_size;

					long duration = frames.buffer[index1].date - frames.buffer[index0].date;
					assert(duration >= 0);
					return float(duration);
				}

				static float FrameDrawCallsPlot(void* data, int i)
				{
					const CircularBuffer<FrameSample>& frames = *(CircularBuffer<FrameSample>*)data;
					int index = frames.headIndex - (100 - i);
					while (index < 0 && frames.buffer.size == frames.buffer.max_size)
					{
						index += frames.buffer.max_size;
					}
					if (index < 0)
					{
						return 0;
					}
					return float(frames.buffer[index].drawCalls);
				}
			};

			const int maxDuration = ComputeMaxFrameDuration();
			ImGui::Text("Frame: %.1f ms - %.1f FPS", averageFrameDuration, (averageFrameDuration != 0 ? 1000.f / averageFrameDuration : 0));
			ImGui::PlotLines("Frame duration (ms)", Local::FrameDurationPlot, &frames, 100, 0, NULL, 0.f, 1.2f * maxDuration, ImVec2(0, 60));

#if DEBUG_FRAME
			const int maxDrawCalls = ComputeMaxFrameDrawCalls();
			ImGui::Text("Drawcalls: %d", drawCallsCount);
			if (controls.stopAtDrawCall > 0)
			{
				ImGui::Text("Showing drawcall #%d", controls.stopAtDrawCall);
			}
			ImGui::PlotHistogram("Frame draw calls", Local::FrameDrawCallsPlot, &frames, 100, 0, NULL, 0.f, 1.2f * maxDrawCalls, ImVec2(0, 60));
#endif // DEBUG_FRAME
		}

		ImGui::End();
	}

	if (seekToDate >= 0)
	{
		clock.AbsoluteSeek(seekToDate);
		MusicPlayerSeek(clock, pPlayer);
	}
}

void UpdateWindowTitle(const platform::Platform& platform,
					   float averageFrameDuration,
					   int drawCallsCount,
					   const Timeline::Clock& clock,
					   const Tool::DebugControls& controls)
{
	std::ostringstream oss;
	oss << "H";

	// Play information:
	{
		if (controls.selectedCamera == Tool::manualCameraMode)
		{
			oss << " - MANUAL";
			if (controls.manualClock.isPaused)
			{
				oss << "(paused)";
			}
		}
#if ENABLE_EDITING_CAMERA
		else if (controls.selectedCamera == Tool::editingCameraMode)
		{
			oss << " - edition";
		}
#endif // ENABLE_EDITING_CAMERA
		else // defaultCameraMode
		{
			int minutes, seconds, milliseconds;
			GetReadableTime(clock.YoutubeTime(), minutes, seconds, milliseconds);

			oss << " - "
				<< minutes << "'"
				<< std::setfill('0') << std::setw(2) << seconds << "\""
				<< std::setfill('0') << std::setw(3) << milliseconds;
		}
	}

	//  Performance information:
	{
		int averageFPS = (int)(averageFrameDuration != 0 ? 1000.f / averageFrameDuration : 0);
		oss << " - " << (int)averageFrameDuration << " ms/frame"
			<< " - " << averageFPS << " fps";
	}

	// Assets information:
	{
		int warnings = Core::log.count[Core::LogLevel::Warning];
		if (warnings > 0)
		{
			oss << " - " << warnings << " warnings";
		}
		int errors = Core::log.count[Core::LogLevel::Error];
		if (errors > 0)
		{
			oss << " - " << errors << " errors";
		}
		int fatals = Core::log.count[Core::LogLevel::Fatal];
		if (fatals > 0)
		{
			oss << " - " << fatals << " fatal errors";
		}
	}

	platform.SetWindowTitle(oss.str().c_str());
}
#endif

void run()
{
	const char* title = "H - Immersion, by Ctrl-Alt-Test";
	Core::Settings settings;

	int monitorLeft;
	int monitorTop;
	int monitorWidth;
	int monitorHeight;
#if defined(ENABLE_LAUNCH_DIALOG) &&  defined(_WIN32)
	platform::Dialog::s_instance = new platform::Dialog();
	platform::Dialog::s_instance->run(title);
	if (!platform::Dialog::s_instance->isOk)
	{
		return;
	}
	settings.renderWidth = platform::Dialog::s_instance->renderWidth;
	settings.renderHeight = platform::Dialog::s_instance->renderHeight;
	if (platform::Dialog::s_instance->fullscreen)
	{
		settings.windowWidth = platform::Dialog::s_instance->monitorWidth;
		settings.windowHeight = platform::Dialog::s_instance->monitorHeight;
	}
	else
	{
		settings.windowWidth = platform::Dialog::s_instance->renderWidth;
		settings.windowHeight = platform::Dialog::s_instance->renderHeight;
	}
	settings.displayAspectRatio = platform::Dialog::s_instance->displayAspectRatio;
	settings.fullscreen = platform::Dialog::s_instance->fullscreen;
#if ENABLE_QUALITY_OPTION
	settings.SetQuality((Core::Settings::GraphicsQuality)platform::Dialog::s_instance->quality);
#endif // ENABLE_QUALITY_OPTION
	monitorLeft = platform::Dialog::s_instance->monitorLeft;
	monitorTop = platform::Dialog::s_instance->monitorTop;
	monitorWidth = platform::Dialog::s_instance->monitorWidth;
	monitorHeight = platform::Dialog::s_instance->monitorHeight;
#else // !ENABLE_LAUNCH_DIALOG || !_WIN32
	monitorLeft = 0;
	monitorTop = 0;
	monitorWidth = 1920;
	monitorHeight = 1080;
#endif // !ENABLE_LAUNCH_DIALOG || !_WIN32

	Timeline::Clock clock;

#if DEBUG
	Tool::DebugControls debugControls;
	debugControls.clock = &clock;
	Tool::DebugControls* pDebugControls = &debugControls;
#else // !DEBUG
#define pDebugControls NULL
#endif // !DEBUG

	platform::Platform platform(title,
								settings.windowWidth, settings.windowHeight,
								monitorLeft, monitorTop,
								monitorWidth, monitorHeight,
								settings.fullscreen);
#if DEBUG
	platform.AddInputHandler(Tool::ImGuiHelper::HandleCharacter, Tool::ImGuiHelper::HandleKey, Tool::ImGuiHelper::HandleMouse);
	platform.AddInputHandler(nullptr, Tool::DebugControls::HandleKey, Tool::DebugControls::HandleMouse);
#endif // DEBUG

#if DEBUG_FRAME
	Gfx::OpenGLLayer realGfxLayer;
	Tool::DebugGraphicLayer* gfxLayer = new Tool::DebugGraphicLayer(realGfxLayer, &debugControls);
#else // !DEBUG_FRAME
	Gfx::IGraphicLayer* gfxLayer = new Gfx::OpenGLLayer();
#endif // !DEBUG_FRAME

	gfxLayer->CreateRenderingContext();
	LoadingBar::init(gfxLayer, &platform, settings.windowWidth, settings.windowHeight);

#if DEBUG
#if DEBUG_FRAME
	Tool::ImGuiHelper::Init(&realGfxLayer);
#else // DEBUG_FRAME
	Tool::ImGuiHelper::Init(gfxLayer);
#endif // DEBUG_FRAME
#endif // DEBUG

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
	runtime::FileWatcher fileWatcher;
	runtime::FileWatcher* pFileWatcher = &fileWatcher;
#else // !ENABLE_AUTOMATIC_FILE_RELOAD
#define pFileWatcher NULL
#endif // ENABLE_AUTOMATIC_FILE_RELOAD

#ifdef ENABLE_RUNTIME_COMPILATION
	runtime::RuntimeCompiler runtimeCompiler;
	runtime::RuntimeCompiler* pRuntimeCompiler = &runtimeCompiler;
#else // !ENABLE_RUNTIME_COMPILATION
#define pRuntimeCompiler NULL
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_REMOTE_CONTROL
	Tool::Server::Init();
#endif // !ENABLE_REMOTE_CONTROL

#ifdef ENABLE_PLAYER_64KLANG2
#ifndef MUTE_SOUND
	MUSIC_PLAYER.Init();
#endif // !MUTE_SOUND
#if DEBUG
	debugControls.player = &MUSIC_PLAYER;
#endif // DEBUG
#endif // !ENABLE_PLAYER_64KLANG2

	H scene(gfxLayer, pDebugControls, pFileWatcher, pRuntimeCompiler);
#if DEBUG
	const long loadStartTime = platform.GetTime();
#endif // DEBUG
	scene.Init(settings);

	LoadingBar::complete();
#if DEBUG
	LOG_DEBUG("Initialized in %d ms", platform.GetTime() - loadStartTime);
#endif // DEBUG

	const long startTime = platform.GetTime();
	long lastFileUpdateTime = 0;
	int drawCallsCount = 0;
#if DEBUG
	long lastTitleUpdateTime = 0;
#endif // DEBUG
	MUSIC_PLAYER.Play();
	while (platform.HandleMessages())
	{
		const long frameStartTime = platform.GetTime();
		const long currentTime = frameStartTime - startTime;
		clock.Update(currentTime);
#if DEBUG
		Tool::ImGuiHelper::NewFrame(settings.renderWidth, settings.renderHeight, 0.016f);
		debugControls.Update(currentTime);
#endif // DEBUG

		int t = clock.YoutubeTime();
#if !DEBUG
		if (t > DEMO_DURATION) break; // quit demo automatically
#endif // !DEBUG
		scene.Draw(t);

#if DEBUG_FRAME
		drawCallsCount = gfxLayer->DrawCallsCount();
#endif // DEBUG_FRAME
#if DEBUG
		float averageFrameDuration = ComputeAverageFrameDuration(20);
		ShowDebugWindow(averageFrameDuration, drawCallsCount, clock, debugControls, &MUSIC_PLAYER);
		ImGui::Render();
#endif // DEBUG
		gfxLayer->EndFrame();
		platform.SwapBuffers();

#if DEBUG
		AddFrameDurationSample(clock, drawCallsCount);
		if (currentTime - lastTitleUpdateTime > 100)
		{
			UpdateWindowTitle(platform, averageFrameDuration, drawCallsCount, clock, debugControls);
			lastTitleUpdateTime = currentTime;
		}
#endif // DEBUG

		if (currentTime - lastFileUpdateTime > 100)
		{
#ifdef ENABLE_TWEAKVAL
			ReloadChangedTweakableValues();
#endif // ENABLE_TWEAKVAL

#ifdef ENABLE_RUNTIME_COMPILATION
			runtimeCompiler.Update(currentTime - lastFileUpdateTime);
#endif // ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD
			fileWatcher.Update();
#endif // ENABLE_AUTOMATIC_FILE_RELOAD

#ifdef ENABLE_REMOTE_CONTROL
			Tool::Server::DoIO(clock.YoutubeTime());
#endif // ENABLE_REMOTE_CONTROL

			lastFileUpdateTime = currentTime;
		}
	}

#if DEBUG
	Tool::ImGuiHelper::Shutdown();
#endif // DEBUG

	gfxLayer->DestroyRenderingContext();

	// Let the OS do the cleaning.
	//delete gfxLayer;
}

int __cdecl main()
{
	LOG_INFO("Starting H");

#if _HAS_EXCEPTIONS
	try
	{
		run();
	}
	catch (std::exception* e)
	{
		// FIXME: ensure we close everything properly (rendering
		//        context and draw context).
		LOG_FATAL(e->what());
		return 1;
	}
#else // !_HAS_EXCEPTIONS
	run();
#endif // !_HAS_EXCEPTIONS

	// FIXME: ensure we quit properly (destroy draw context, post quit
	//        message).
	LOG_INFO("Ending H");
	return 0;
}

#if _WIN32
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	return main();
}
#endif // _WIN32
