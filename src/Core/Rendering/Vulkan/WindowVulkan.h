#pragma once
#include "Core/Rendering/Window.h"
#include <XPlatform/access.h>

TRISTEON_UNIQUE_ACCESS_DECL()
namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief Vulkan::Window overrides some of the basic behavior of Window to fit the purposes of Vulkan
				 */
				class Window final : public Rendering::Window
				{
					TRISTEON_UNIQUE_ACCESS(Window)
				protected:
					/**
					 * \brief Sets up Vulkan specific GLFW parameters
					 */
					void onPreWindowCreation() override;

					Window() = default;
					~Window() = default;
				};
			}
		}
	}
}
