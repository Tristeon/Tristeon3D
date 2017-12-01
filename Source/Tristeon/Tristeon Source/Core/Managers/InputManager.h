#pragma once
#include "Manager.h"

//Forward decl
struct GLFWwindow;

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class Message;
		class ManagerProtocol;

		namespace Managers
		{
			/**
			 * \brief InputManager manages all the input calls coming from the GLFW window and converts them to Mouse/Keyboard/Joystick data
			 */
			class InputManager final : public Manager
			{
			public:
				/**
				 * \brief Creates an InputManager instance
				 * \param window The GLFW window, used to subscribe to callbacks 
				 */
				explicit InputManager(GLFWwindow* window);
				/**
				 * \brief Subscribes to all the callbacks
				 */
				void init() override;
			private:
				/**
				 * \brief Standard manager reset function, InputManager doesn't use reset, so we'll leave it empty
				 */
				void reset() override { /*Empty*/ }
				/**
				 * \brief Resets keydown and keyup. Automatically registers to the after frame callback
				 */
				void resetInput() const;

				/**
				 * \brief A reference to GLFWwindow, used for input callbacks
				 */
				GLFWwindow* window;
			};
		}
	}
}
