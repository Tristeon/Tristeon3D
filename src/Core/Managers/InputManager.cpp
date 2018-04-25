#include "InputManager.h"
#include "Core/ManagerProtocol.h"

#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Mouse.h"

#include <GLFW/glfw3.h>

namespace Tristeon
{
	namespace Core
	{
		namespace Managers
		{
			InputManager::InputManager(GLFWwindow* window) : window(window)
			{
				//Empty
			}

			void InputManager::init()
			{
				//Resets keydown and keyup
				ManagerProtocol::subscribeToMessage(MT_AFTERFRAME, [&](Message msg) { resetInput(); });
				
				//GLFW input callbacks
				Misc::Mouse::window = window;
				glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) { Misc::Keyboard::keyCallback(key, scancode, action, mods); });
				glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) { Misc::Mouse::buttonCallback(button, action, mods); });
				glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) { Misc::Mouse::positionCallback(x, y); });
				glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int action) { Misc::Mouse::windowEnterCallback(static_cast<bool>(action)); });
				glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) { Misc::Mouse::scrollCallback(x, y); });
			}

			void InputManager::resetInput() const
			{
				//Reset keydown and key up
				Misc::Keyboard::reset();
				Misc::Mouse::reset();
			}
		}
	}
}