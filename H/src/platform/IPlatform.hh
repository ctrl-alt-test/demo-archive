#ifndef IPLATFORM_HH
#define IPLATFORM_HH

#include "KeyCodes.hh"
#include "gfx/IGraphicLayer.hh"

namespace platform
{
	/// <summary>
	/// Character input callback.
	/// Returns true if the event is processed and should not be passed
	/// through, false otherwise.
	/// </summary>
	typedef bool (*CharacterHandler)(unsigned short symbol);

	/// <summary>
	/// Keyboard input callback.
	/// Returns true if the event is processed and should not be passed
	/// through, false otherwise.
	/// </summary>
	typedef bool (*KeyHandler)(KeyCode::Enum key, bool pressed);

	/// <summary>
	/// Mouse input callback.
	/// Returns true if the event is processed and should not be passed
	/// through, false otherwise.
	/// </summary>
	typedef bool (*MouseHandler)(int x, int y, int wheel, bool leftDown, bool rightDown, bool middleDown, bool x1Down, bool x2Down);

	//
	// Unused.
	// Just here to show what a platform class is expected to implement.
	//

#if 0
	class IPlatform
	{
	public:
		virtual ~IPlatform() {}

		int		GetWidth() const = 0;
		int		GetHeight() const = 0;

		long	GetTime() const = 0;
		void	Sleep(long milliseconds) const = 0;
		void	SetWindowTitle(const char* title) const = 0;
		void	AddInputHandler(CharacterHandler characterHandler, KeyHandler keyHandler, MouseHandler mouseHandler) = 0;
		bool	HandleMessages() const = 0;
		void	SwapBuffers() const = 0;
	};
#endif
}

#endif // IPLATFORM_HH
