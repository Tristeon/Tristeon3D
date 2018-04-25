#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				/**
				 * \brief Helper class for one time submit vulkan command buffers.
				 */
				class CommandBuffer
				{
				public:
					/**
					 * \brief Start a one time submit command buffer
					 * \param commandPool The command pool to allocate the buffer off
					 * \param device The vulkan device to allocate a commandbuffer
					 * \return Returns a vk::Commandbuffer that can be passed to CommandBuffer::end() once you're done using this commandbuffer.
					 */
					static vk::CommandBuffer begin(vk::CommandPool commandPool, vk::Device device);
					/**
					 * \brief Ends, submits and destroys the one time command buffer
					 * \param buffer The command buffer that was obtained before by using CommandBuffer::begin()
					 * \param graphicsQueue The graphics queue to submit the buffer to
					 * \param device The vulkan device to free the commandbuffer
					 * \param cmdPool The command pool to free the commandbuffer from
					 */
					static void end(vk::CommandBuffer buffer, vk::Queue graphicsQueue, vk::Device device, vk::CommandPool cmdPool);
				};
			}
		}
	}
}
