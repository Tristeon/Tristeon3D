#pragma once
#include <Math/Vector2.h>
#include "Misc/Delegate.h"
#include "Keyboard.h"

#define MAXBUTTONS 7

struct GLFWwindow;

namespace Tristeon
{
	namespace Data { class Image; }
	namespace Core { namespace Managers { class InputManager; } }

	namespace Misc
	{
		/**
		 * Every available mouse button in Tristeon.
		 * Used in conjunction with GetButton(), GetButtonUp() and GetButtonDown()
		 */
		enum MouseButton
		{
			BUTTON_1 = 0,
			BUTTON_2 = 1,
			BUTTON_3 = 2,
			BUTTON_4 = 3,
			BUTTON_5 = 4,
			BUTTON_6 = 5,
			BUTTON_7 = 6,
			BUTTON_8 = 7,
			BUTTON_LAST = BUTTON_8,
			BUTTON_LEFT = BUTTON_1,
			BUTTON_RIGHT = BUTTON_2,
			BUTTON_MIDDLE = BUTTON_3
		};

		/**
		 * Standard cursor shapes like arrow, crosshair and hand
		 */
		enum CursorShape
		{
			/**
			 * The regular arrow cursor shape.
			 */
			ARROW = 0x00036001,
			/**
			 * The text input I-beam cursor shape.
			 */
			IBEAM = 0x00036002,
			/**
			 * The crosshair shape.
			 */
			CROSSHAIR = 0x00036003,
			/**
			 * The hand shape.
			 */
			HAND = 0x00036004,
			/**
			 * The horizontal resize arrow shape.
			 */
			HRESIZE = 0x00036005,
			/**
			 * The vertical resize arrow shape.
			 */
			VRESIZE = 0x00036006
		};

		/**
		 * The cursor mode
		 */
		enum CursorMode
		{
			/**
			 * The cursor is visible and behaves like normal
			 */
			NORMAL = 0x00034001,
			/**
			 * Makes the cursor invisible when it is over the client area of the window but does not restrict the cursor from leaving.
			 */
			HIDDEN = 0x00034002,

			/**
			 * Hides and grabs the cursor, providing virtual and unlimited cursor movement. This is useful for implementing for example 3D camera controls.
			 */
			DISABLED = 0x00034003
		};

		/**
		 * The interface to get mouse input information from Tristeon
		 */
		class Mouse
		{
			friend Core::Managers::InputManager;
		public:
			/**
			 * Callback for when a button is pressed
			 */
			static Delegate<MouseButton, KeyMods> onButtonDown;
			/**
			 * Callback for when a button is released
			 */
			static Delegate<MouseButton, KeyMods> onButtonUp;

			/**
			 * Callback for when the mouse enters the window
			 */
			static Delegate<> onEnteredWindow;
			/**
			 * Callback for when the mouse exits the window
			 */
			static Delegate<> onExitedWindow;

			/**
			 * Returns true during the frame the user pressed down the given button
			 */
			static bool getButtonDown(MouseButton button);
			/**
			 * Returns true while the given button is being held down
			 */
			static bool getButton(MouseButton button);
			/**
			 * Returns true during the frame the user pressed down the given button
			 */
			static bool getButtonUp(MouseButton button);

			/**
			 * Returns true if the mouse is inside of the window
			 */
			static bool getInWindow();
			
			/**
			 * Returns whether any mouse button has been pressed
			 */
			static bool anyButton();

			/**
			 * Gets the position of the mouse relative to the window
			 */
			static Math::Vector2 getPosition();
			/**
			 * Gets the delta movement of the mouse in the current frame
			 */
			static Math::Vector2 getMouseDelta();
			/**
			 * Gets the scroll movement of the mouse in the current frame
			 */
			static Math::Vector2 getScroll();

			/**
			 * Sets the cursor image. The cursor will only be visible when hovering over the Tristeon window and when the CursorMode is set to CursorMode::NORMAL
			 * \param image The desired cursor image
			 * \param xHot The desired x coordinate, in pixels, of the cursor hotspot
			 * \param yHot The desired y coordinate, in pixels, of the cursor hotspot
			 * 
			 * \exception invalid_argument If the image.width or height is not more than 0
			 */
			static void setCursorImage(Data::Image image, int xHot = 0, int yHot = 0);

			/**
			 * Sets the cursor image to a default shape. The cursor will only be visible when hovering over the Tristeon window and when the CursorMode is set to CursorMode::NORMAL
			 */
			static void setCursorDefault(CursorShape shape);

			/**
			 * Sets the cursor mode (hidden, disabled, normal). Describes visibility and usage of the cursor
			 */
			static void setCursorMode(CursorMode mode);
		private:
			static bool buttonsDown[MAXBUTTONS];
			static bool buttons[MAXBUTTONS];
			static bool buttonsUp[MAXBUTTONS];
			static bool inWindow;

			/**
			 * Resets the pressed down and released buttons arrays
			 */
			static void reset();

			static void buttonCallback(int button, int action, int mods);
			static void positionCallback(int x, int y);
			static void windowEnterCallback(bool enter);
			static void scrollCallback(double x, double y);

			static Math::Vector2 position;
			static Math::Vector2 deltaPos;
			static Math::Vector2 scroll;
			static GLFWwindow* window;
		};
	}
}