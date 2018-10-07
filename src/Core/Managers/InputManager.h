#pragma once
#include <XPlatform/access.h>

struct GLFWwindow;

TRISTEON_UNIQUE_ACCESS_DECL()

namespace Tristeon
{
	namespace Core
	{
		class Engine;

		namespace Managers
		{
			/**
			 * \brief InputManager manages all the input calls coming from the window context and caches them for usage as Mouse/Keyboard/Joystick data
			 *
			 * This class is not intended to be accessed or used by users.
			 */
			class InputManager final
			{
				TRISTEON_UNIQUE_ACCESS(InputManager)
				/**
				 * \param window The GLFW window, used to subscribe to callbacks 
				 */
				explicit InputManager(GLFWwindow* window);
				/**
				 * \brief Resets keydown and keyup, called every frame
				 */
				void resetInput() const;
			};
		}
	}
}
