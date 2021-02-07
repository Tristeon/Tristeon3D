#pragma once
#include "Core/Rendering/Window.h"

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
				public:
					Window() = default;
					~Window() = default;
					
				protected:
					/**
					 * \brief Sets up Vulkan specific GLFW parameters
					 */
					void onPreWindowCreation() override;
				};
			}
		}
	}
}
