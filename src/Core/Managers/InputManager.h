#pragma once

struct GLFWwindow;

namespace Tristeon
{
	namespace Core
	{
		class Engine;

		namespace Managers
		{
			/**
			 * \brief InputManager manages all the input calls coming from the window context and converts them to Mouse/Keyboard/Joystick data
			 */
			class InputManager final
			{
				friend Engine;

			public:
				/**
				 * \param window The GLFW window, used to subscribe to callbacks 
				 */
				explicit InputManager(GLFWwindow* window);
			private:
				/**
				 * \brief Resets keydown and keyup, called every frame
				 */
				void resetInput() const;
			};
		}
	}
}
