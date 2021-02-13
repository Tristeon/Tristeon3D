#include "InputManager.h"
#include "Core/MessageBus.h"

#include "Misc/Hardware/Keyboard.h"
#include "Misc/Hardware/Mouse.h"

#include <GLFW/glfw3.h>
#include "Core/BindingData.h"

namespace Tristeon::Core::Managers
{
	InputManager::InputManager()
	{
		GLFWwindow* window = binding_data.window;
				
		MessageBus::subscribe(Message::Type::AfterFrame, [&](Message msg) { resetInput(); });
		Misc::Mouse::window = window;
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) { Misc::Keyboard::keyCallback(key, scancode, action, mods); });
		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) { Misc::Mouse::buttonCallback(button, action, mods); });
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) { Misc::Mouse::positionCallback(x, y); });
		glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int action) { Misc::Mouse::windowEnterCallback(static_cast<bool>(action)); });
		glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) { Misc::Mouse::scrollCallback(x, y); });
	}

	void InputManager::resetInput()
	{
		Misc::Keyboard::reset();
		Misc::Mouse::reset();
	}
}
