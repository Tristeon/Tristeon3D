#pragma once
#include <Math/Vector2.h>
#include "Misc/Delegate.h"
#include "Keyboard.h"

#define MAXBUTTONS 7

//Forward decl
struct GLFWwindow;

namespace Tristeon
{
	namespace Data {
		class Image;
	}

	//Forward decl
	namespace Core { namespace Managers { class InputManager; } }

	namespace Misc
	{
		/**
		 * \brief MouseButton describes every available mousebutton in Tristeon.
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
		 * \brief Defines standard cursor shapes like arrow and hand
		 */
		enum CursorShape
		{
			/**
			 * \brief The regular arrow cursor shape.
			 */
			ARROW = 0x00036001,
			/**
			 * \brief The text input I-beam cursor shape.
			 */
			IBEAM = 0x00036002,
			/**
			 * \brief The crosshair shape.
			 */
			CROSSHAIR = 0x00036003,
			/**
			 * \brief The hand shape.
			 */
			HAND = 0x00036004,
			/**
			 * \brief The horizontal resize arrow shape.
			 */
			HRESIZE = 0x00036005,
			/**
			 * \brief The vertical resize arrow shape.
			 */
			VRESIZE = 0x00036006
		};

		/**
		 * \brief Describes the cursor input mode
		 */
		enum CursorMode
		{
			/**
			 * \brief The cursor is visible and behaves like normal
			 */
			NORMAL = 0x00034001,
			/**
			 * \brief Makes the cursor invisible when it is over the client area of the window but does not restrict the cursor from leaving.
			 */
			HIDDEN = 0x00034002,

			/**
			 * \brief Hides and grabs the cursor, providing virtual and unlimited cursor movement. This is useful for implementing for example 3D camera controls.
			 */
			DISABLED = 0x00034003
		};

		/**
		 * \brief The interface to get mouse input information from Tristeon
		 */
		class Mouse
		{
			//InputManager can change our private variables
			friend Core::Managers::InputManager;

		public:
			/**
			 * \brief Callback for when a button is pressed
			 */
			static Delegate<MouseButton, KeyMods> onButtonDown;
			/**
			 * \brief Callback for when a button is released
			 */
			static Delegate<MouseButton, KeyMods> onButtonUp;

			/**
			 * \brief Callback for when the mouse enters the window
			 */
			static Delegate<> onEnteredWindow;
			/**
			 * \brief Callback for when the mouse exits the window
			 */
			static Delegate<> onExitedWindow;

			/**
			 * \brief Returns true during the frame the user pressed down the given button
			 * \param button The button to be checked
			 * \return Returns true or false 
			 */
			static bool getButtonDown(MouseButton button);
			/**
			 * \brief Returns true while the given button is being held down
			 * \param button The button to be checked
			 * \return Returns true or false
			 */
			static bool getButton(MouseButton button);
			/**
			 * \brief Returns true during the frame the user pressed down the given button
			 * \param button The button to be checked
			 * \return Returns true or false
			 */
			static bool getButtonUp(MouseButton button);

			/**
			 * \brief Returns true if the mouse is inside of the window
			 * \return Returns true or false
			 */
			static bool getInWindow();

			/**
			 * \return Gets the position of the mouse relative to the window
			 */
			static Math::Vector2 getPosition();
			/**
			 * \brief Gets the delta movement of the mouse in the current frame
			 * \return Returns a vector describing the delta movement
			 */
			static Math::Vector2 getMouseDelta();
			/**
			 * \brief Gets the scroll movement of the mouse in the current frame
			 * \return Returns a vector describing the delta scroll
			 */
			static Math::Vector2 getScroll();

			/**
			 * \brief Sets the cursor image. The cursor will only be visible when hovering over the Tristeon window and when the CursorMode is set to CursorMode::NORMAL
			 * \param image The desired cursor image
			 * \param xHot The desired x coordinate, in pixels, of the cursor hotspot
			 * \param yHot The desired y coordinate, in pixels, of the cursor hotspot
			 */
			static void setCursorImage(Data::Image image, int xHot = 0, int yHot = 0);

			/**
			 * \brief Sets the cursor image to a default shape. The cursor will only be visible when hovering over the Tristeon window and when the CursorMode is set to CursorMode::NORMAL
			 * \param shape The shape of the cursor
			 */
			static void setCursorDefault(CursorShape shape);

			/**
			 * \brief Sets the cursor mode (hidden, disabled, normal). Describes visibility and usage of the cursor
			 * \param mode The cursor mode
			 */
			static void setCursorMode(CursorMode mode);
		private:
			/**
			 * \brief Currently pressed down buttons
			 */
			static bool buttonsDown[MAXBUTTONS];
			/**
			 * \brief Currently held down buttons
			 */
			static bool buttons[MAXBUTTONS];
			/**
			 * \brief Currently released buttons
			 */
			static bool buttonsUp[MAXBUTTONS];
			/**
			 * \brief A boolean describing wether or not the mouse is inside of the window
			 */
			static bool inWindow;

			/**
			 * \brief Resets the pressed down and released buttons arrays
			 */
			static void reset();

			/**
			 * \brief A function callback that is used to record button input data
			 * \param button The button who's state has changed
			 * \param action The action (pressed, continued, released)
			 * \param mods The key modifiers applied to this key (ctrl, shift, alt, etc)
			 */
			static void buttonCallback(int button, int action, int mods);
			/**
			 * \brief A function callback that is used to record mouse position data 
			 * \param x The new x position of the mouse
			 * \param y The new y position of the mouse
			 */
			static void positionCallback(int x, int y);
			/**
			 * \brief A function callback that is used to record when the mouse has entered or exited the window
			 * \param enter A boolean describing wether or not the mouse is inside of the window
			 */
			static void windowEnterCallback(bool enter);
			/**
			 * \brief A function callback that is used to record mouse scroll input data
			 * \param x The x scroll movement
			 * \param y The y scroll movement
			 */
			static void scrollCallback(double x, double y);

			/**
			 * \brief The current position of the mouse
			 */
			static Math::Vector2 position;
			/**
			 * \brief The delta movement of the mouse in the current frame
			 */
			static Math::Vector2 deltaPos;
			/**
			 * \brief The scroll movement of the mouse in the current frame
			 */
			static Math::Vector2 scroll;

			/**
			 * \brief A reference to GLFW window, used to set/get data where needed
			 */
			static GLFWwindow* window;
		};
	}
}