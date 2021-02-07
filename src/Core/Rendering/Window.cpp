#include "Window.h"

#include <Core/Message.h>
#include <Core/MessageBus.h>
#include <GLFW/glfw3.h>
#include <Math/Vector2.h>
#include <Misc/Console.h>

namespace Tristeon::Core::Rendering
{
	Window::~Window()
	{
		MessageBus::sendMessage(MT_QUIT);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::init()
	{
		//Error callback has to be set before anything else, to not miss out on potential early errors
		glfwSetErrorCallback([](int error, const char* description) { Misc::Console::error("GLFW Error. Error code: " + std::to_string(error) + ". Description: " + description); });

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_RESIZABLE, true);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorPhysicalSize(monitor, &width.value, &height.value);

		//Create and call callbacks
		onPreWindowCreation();
		window.value = glfwCreateWindow(width, height, "Tristeon", nullptr, nullptr);
		if (window.get() == nullptr)
			Misc::Console::error("Failed to open GLFW window!");
		glfwMaximizeWindow(window);
		glfwSetWindowUserPointer(window.get(), this);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) { static_cast<Window*>(glfwGetWindowUserPointer(window))->onResize(w, h); });
		onPostWindowCreation();
	}

	void Window::onResize(int width, int height)
	{
		this->width.value = width;
		this->height.value = height;
		Math::Vector2 size = Math::Vector2((float)width, (float)height);
		MessageBus::sendMessage(Message(MT_WINDOW_RESIZE, &size));
	}
}
