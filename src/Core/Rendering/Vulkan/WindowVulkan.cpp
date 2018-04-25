#include "WindowVulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				void Window::onPreWindowCreation()
				{
					//Tell GLFW to not create an OpenGL context
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				}
			}
		}
	}
}