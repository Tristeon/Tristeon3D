#include "Window.h"

#include <Core/Message.h>
#include <Core/MessageBus.h>
#include <GLFW/glfw3.h>
#include <Math/Vector2.h>
#include <Misc/Console.h>
#include <Core/RenderData.h>

namespace Tristeon::Core::Rendering
{
	Window::~Window()
	{
		glfwDestroyWindow(renderData.window);
		glfwTerminate();
	}

	Window::Window()
	{
		//Error callback has to be set before anything else, to not miss out on potential early errors
		glfwSetErrorCallback([](int error, const char* description) { Misc::Console::error("[GLFW] [ERROR] [CODE: " + std::to_string(error) + "] Description: " + description); });

		if (!glfwInit())
		{
			Misc::Console::error("[WINDOW] [ERROR] Failed to initialize GLFW");
			return;
		}
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_RESIZABLE, true);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorPhysicalSize(monitor, &width.value, &height.value);

		//Create and call callbacks
		Misc::Console::write("[WINDOW] [INIT] Creating GLFW window");
		renderData.window = glfwCreateWindow(width, height, "Tristeon", nullptr, nullptr);
		if (renderData.window == nullptr)
			Misc::Console::error("Failed to open GLFW window!");
		glfwMaximizeWindow(renderData.window);
		glfwSetWindowUserPointer(renderData.window, this);

		Misc::Console::write("[WINDOW] [INIT] Binding GLFW callbacks");
		glfwSetWindowSizeCallback(renderData.window, [](GLFWwindow* window, int w, int h) { static_cast<Window*>(glfwGetWindowUserPointer(window))->onResize(w, h); });
	}

	void Window::onResize(int width, int height)
	{
		Misc::Console::write("[WINDOW] [EVENT] Resizing window to " + std::to_string(width) + "x" + std::to_string(height));
		
		this->width.value = width;
		this->height.value = height;
		Math::Vector2 size = Math::Vector2((float)width, (float)height);
		MessageBus::send(Message(Message::Type::WindowResize, &size));
	}
}