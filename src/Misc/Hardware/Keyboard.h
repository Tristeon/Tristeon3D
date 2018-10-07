#pragma once
#include "Misc/Delegate.h"

/**
 * Defines the maximum amount of keys supported. Any other key will be defined as unknown (-1)
 */
#define MAXKEYS 348

struct GLFWwindow;
namespace Tristeon
{
	namespace Core { namespace Managers { class InputManager; } }
	namespace Misc
	{
		/**
		 * Bits describing key modifiers (shift, control, etc)
		 */
		enum KeyMods
		{
			/** 
			 * If this bit is set one or more Shift keys were held down.
			*/
			SHIFT = 0x0001,
			/**
			 * If this bit is set one or more Control keys were held down.
			*/
			CONTROL = 0x0002,
			/**
			 * If this bit is set one or more Alt keys were held down.
			*/
			ALT = 0x0004,
			/**
			 * If this bit is set one or more Super keys were held down.
			*/
			SUPER = 0x0008
		};

		/**
		 * All the supported keyboard keys
		 */
		enum KeyCode
		{
			/**
			 * KeyCode for unsupported keys
			 */
			UNKNOWN = -1,
			SPACE = 32,
			APOSTROPHE = 39, /* ' */
			COMMA = 44, /* , */
			MINUS = 45, /* - */
			PERIOD = 46, /* . */
			SLASH = 47, /* / */

			NUM_0 = 48,
			NUM_1 = 49,
			NUM_2 = 50,
			NUM_3 = 51,
			NUM_4 = 52,
			NUM_5 = 53,
			NUM_6 = 54,
			NUM_7 = 55,
			NUM_8 = 56,
			NUM_9 = 57,

			SEMICOLON = 59, /* ; */
			EQUAL = 61, /* = */

			A = 65,
			B = 66,
			C = 67,
			D = 68,
			E = 69,
			F = 70,
			G = 71,
			H = 72,
			I = 73,
			J = 74,
			K = 75,
			L = 76,
			M = 77,
			N = 78,
			O = 79,
			P = 80,
			Q = 81,
			R = 82,
			S = 83,
			T = 84,
			U = 85,
			V = 86,
			W = 87,
			X = 88,
			Y = 89,
			Z = 90,

			LEFT_BRACKET = 91, /* [ */
			BACKSLASH = 92, /* \ */
			RIGHT_BRACKET = 93, /* ] */
			GRAVE = 96, /* ` */

			WORLD_1 = 161, /* non-US #1 */
			WORLD_2 = 162, /* non-US #2 */

			ESCAPE = 256,
			ENTER = 257,
			TAB = 258,
			BACKSPACE = 259,
			INSERT = 260,
			DELETE = 261,
			RIGHT = 262,
			LEFT = 263,
			DOWN = 264,
			UP = 265,
			PAGE_UP = 266,
			PAGE_DOWN = 267,
			HOME = 268,
			END = 269,
			CAPS_LOCK = 280,
			SCROLL_LOCK = 281,
			NUM_LOCK = 282,
			PRINT_SCREEN = 283,
			PAUSE = 284,
			F1 = 290,
			F2 = 291,
			F3 = 292,
			F4 = 293,
			F5 = 294,
			F6 = 295,
			F7 = 296,
			F8 = 297,
			F9 = 298,
			F10 = 299,
			F11 = 300,
			F12 = 301,
			F13 = 302,
			F14 = 303,
			F15 = 304,
			F16 = 305,
			F17 = 306,
			F18 = 307,
			F19 = 308,
			F20 = 309,
			F21 = 310,
			F22 = 311,
			F23 = 312,
			F24 = 313,
			F25 = 314,
			KP_0 = 320,
			KP_1 = 321,
			KP_2 = 322,
			KP_3 = 323,
			KP_4 = 324,
			KP_5 = 325,
			KP_6 = 326,
			KP_7 = 327,
			KP_8 = 328,
			KP_9 = 329,
			KP_DECIMAL = 330,
			KP_DIVIDE = 331,
			KP_MULTIPLY = 332,
			KP_SUBTRACT = 333,
			KP_ADD = 334,
			KP_ENTER = 335,
			KP_EQUAL = 336,
			LEFT_SHIFT = 340,
			LEFT_CONTROL = 341,
			LEFT_ALT = 342,
			LEFT_SUPER = 343,
			RIGHT_SHIFT = 344,
			RIGHT_CONTROL = 345,
			RIGHT_ALT = 346,
			RIGHT_SUPER = 347,
			MENU = 348,

			/**
			 * The last supported key. For iteration purposes
			 */
			LAST = MENU
		};

		/**
		 * The interface to get keyboard input information from Tristeon
		 */
		class Keyboard
		{
			friend Core::Managers::InputManager;
		public:
			/**
			 * Returns true if the given key has been pressed down during the current frame
			 */
			static bool getKeyDown(KeyCode key);
			/**
			 * Returns true if the given key while the given key is being held down
			 */
			static bool getKey(KeyCode key);
			/**
			 * Returns true if the given key has been released during the current frame
			 */
			static bool getKeyUp(KeyCode key);

			/**
			 * The user callback for when a key gets pressed down
			 */
			static Delegate<KeyCode, KeyMods> onKeyDown;
			/**
			 * The user callback for when a key gets released
			 */
			static Delegate<KeyCode, KeyMods> onKeyUp;

		private:
			static bool keysDown[MAXKEYS];
			static bool keys[MAXKEYS];
			static bool keysUp[MAXKEYS];

			/**
			 * Resets the keysDown and keysUp arrays
			 */
			static void reset();

			/**
			 * A function callback that is used to record keyboard input data. 
			 * \param key The key that changed state
			 * \param scancode The system-specific scancode of the key.
			 * \param action The action that has been performed on the button
			 * \param mods The key modifiers that are being applied to the button (ctrl, alt, shift, etc)
			 */
			static void keyCallback(int key, int scancode, int action, int mods);
		};
	}
}