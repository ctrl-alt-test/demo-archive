#ifndef KEY_CODES_HH
#define KEY_CODES_HH

namespace platform
{
	struct KeyCode
	{
		enum Enum {
			keyArrowLeft = 0,
			keyArrowRight,
			keyArrowUp,
			keyArrowDown,

			keyLeftAlt,
			keyRightAlt,
			keyLeftControl,
			keyRightControl,
			keyLeftShift,
			keyRightShift,
			keyLeftCommand,
			keyRightCommand,
			keyMenu,

			keyEscape,
			keyEnter,
			keySpace,
			keyTab,
			keyBackspace,

			keyPrintScreen,
			keyScrollLock,
			keyPause,
			keyCapsLock,
			keyNumLock,

			keyInsert,
			keyDelete,
			keyHome,
			keyEnd,
			keyPageUp,
			keyPageDown,

			keyNumpad0,
			keyNumpad1,
			keyNumpad2,
			keyNumpad3,
			keyNumpad4,
			keyNumpad5,
			keyNumpad6,
			keyNumpad7,
			keyNumpad8,
			keyNumpad9,
			keyNumpadDecimal,
			keyNumpadMultiply,
			keyNumpadPlus,
			keyNumpadMinus,
			keyNumpadDivide,

			keyF1,
			keyF2,
			keyF3,
			keyF4,
			keyF5,
			keyF6,
			keyF7,
			keyF8,
			keyF9,
			keyF10,
			keyF11,
			keyF12,
			keyF13,
			keyF14,
			keyF15,
			keyF16,
			keyF17,
			keyF18,
			keyF19,
			keyF20,
			keyF21,
			keyF22,
			keyF23,
			keyF24,

			key0,
			key1,
			key2,
			key3,
			key4,
			key5,
			key6,
			key7,
			key8,
			key9,
			keyPlus,
			keyMinus,

			keyA,
			keyB,
			keyC,
			keyD,
			keyE,
			keyF,
			keyG,
			keyH,
			keyI,
			keyJ,
			keyK,
			keyL,
			keyM,
			keyN,
			keyO,
			keyP,
			keyQ,
			keyR,
			keyS,
			keyT,
			keyU,
			keyV,
			keyW,
			keyX,
			keyY,
			keyZ,

			keyComma,
			keyPeriod,

			numberOfKeys,
		};
	};

	struct KeyEvent
	{
		KeyCode::Enum	key;
		bool			pressed;
	};
}

#endif // KEY_CODES_HH
