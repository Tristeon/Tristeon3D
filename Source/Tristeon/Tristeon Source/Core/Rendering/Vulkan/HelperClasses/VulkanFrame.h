#pragma once
#include <cstdint>

//Forward decl
namespace vk {
	class Queue;
	class Device;
	class SwapchainKHR;
	class Semaphore;
}

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief VulkanFrame is a helper class for preparing / submitting swapchain frames
				 */
				class VulkanFrame
				{
				public:
					/**
					 * \brief Prepares the frame for rendering
					 * \param device The vulkan logical device
					 * \param swapchain The swapchain
					 * \param imageAvailable The image available semaphore, used to request the next image available
					 * \return Returns the current image index
					 */
					static int prepare(vk::Device device, vk::SwapchainKHR swapchain, vk::Semaphore imageAvailable);
					/**
					 * \brief Submits the frame to the present queu
					 * \param device The vulkan logical device
					 * \param swapchain The swapchain
					 * \param renderFinished The render finished semaphore, used to wait till we are able to present the next image
					 * \param presentQueue The present queue
					 * \param index The index of the current active image, returned by prepare()
					 */
					static void submit(vk::Device device, vk::SwapchainKHR swapchain, vk::Semaphore renderFinished, vk::Queue presentQueue, const uint32_t index);
				};
			}
		}
	}
}
