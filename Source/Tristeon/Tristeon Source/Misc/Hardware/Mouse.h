#pragma once
#include <Math/Vector2.h>
#include "Misc/Delegate.h"
#include "Keyboard.h"

#define MAXBUTTONS 7

//Forward decl
struct GLFWwindow;

namespace Tristeon
{
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
			 * \brief Gets the position of the mouse relative to the window
			 * \return 
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
		};
	}
}