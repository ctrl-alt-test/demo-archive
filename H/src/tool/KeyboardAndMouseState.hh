#ifndef MOUSE_AND_KEYBOARD_HH
#define MOUSE_AND_KEYBOARD_HH

#include "platform/KeyCodes.hh"

namespace Tool
{
	struct KeyboardState
	{
		bool	leftCtrlKeyDown;
		bool	rightCtrlKeyDown;
		bool	leftShiftKeyDown;
		bool	rightShiftKeyDown;
		bool	rightAltKeyDown;

		void ReadEvent(const platform::KeyEvent& event)
		{
			if (event.key == platform::KeyCode::keyLeftControl)
			{
				leftCtrlKeyDown = event.pressed;
			}
			if (event.key == platform::KeyCode::keyLeftShift)
			{
				leftShiftKeyDown = event.pressed;
			}
			if (event.key == platform::KeyCode::keyRightControl)
			{
				rightCtrlKeyDown = event.pressed;
			}
			if (event.key == platform::KeyCode::keyRightShift)
			{
				rightShiftKeyDown = event.pressed;
			}
			if (event.key == platform::KeyCode::keyRightAlt)
			{
				rightAltKeyDown = event.pressed;
			}
		}
	};

	struct MouseState
	{
		int		x;
		int		y;
		int		wheel;
		bool	left;
		bool	right;
		bool	middle;
		bool	x1;
		bool	x2;

		static MouseState MouseDiff(const MouseState& a, const MouseState& b)
		{
			MouseState diff = {
				b.x - a.x,
				b.y - a.y,
				b.wheel - a.wheel,
				b.left != a.left,
				b.right != a.right,
				b.middle != a.middle,
				b.x1 != a.x1,
				b.x2 != a.x2, };
			return diff;
		}
	};
}

#endif // !MOUSE_AND_KEYBOARD_HH
