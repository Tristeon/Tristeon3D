#pragma once
#include <vulkan/vulkan.hpp>

namespace Tristeon::Core::Rendering
{
	/**
	 * \brief Helper class for one time submit vulkan command buffers.
	 */
	class OneTimeCmd
	{
	public:
		/**
		 * \brief Start a one time submit command buffer
		 * \param commandPool The command pool to allocate the buffer off
		 * \return Returns a vk::Commandbuffer that can be passed to CommandBuffer::end() once you're done using this commandbuffer.
		 */
		static vk::CommandBuffer begin(const vk::CommandPool& commandPool);
		/**
		 * \brief Ends, submits and destroys the one time command buffer
		 * \param buffer The command buffer that was obtained before by using CommandBuffer::begin()
		 * \param queue The queue to submit the buffer to
		 * \param pool Pool to deallocate the buffer from. Should be the same as the one passed to begin(). 
		 */
		static void end(const vk::CommandBuffer& buffer, const vk::Queue& queue, const vk::CommandPool& pool);
	};
}