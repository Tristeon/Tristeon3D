#include "Window.h"

#include "Core/UserPrefs.h"
#include <Core/MessageBus.h>
#include "Core/Message.h"

#include "Math/Vector2.h"

#include "Misc/Console.h"

#include <GLFW/glfw3.h>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
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

				//Get requested window size
				GLFWmonitor* monitor = nullptr;
				if (UserPrefs::getBoolValue("FULLSCREEN"))
				{
					monitor = glfwGetPrimaryMonitor();
					int width, height;
					glfwGetMonitorPhysicalSize(monitor, &width, &height);
					this->width.value = width;
					this->height.value = height;
				}
				else
				{
					this->width.value = UserPrefs::getIntValue("SCREENWIDTH");
					this->height.value = UserPrefs::getIntValue("SCREENHEIGHT");
				}

				//Create and call callbacks
				onPreWindowCreation();
				window.value = glfwCreateWindow(width, height, "Tristeon", monitor, nullptr);
				if (window.get() == nullptr)
					Misc::Console::error("Failed to open GLFW window!");
				glfwMaximizeWindow(window);
				glfwSetWindowUserPointer(window.get(), this);

				glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) { reinterpret_cast<Window*>(glfwGetWindowUserPointer(w))->onResize(width, height); });
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
	}
}
