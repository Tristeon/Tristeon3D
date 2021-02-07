#include "CommandBuffer.h"
#include <vulkan/vulkan.hpp>

#include "Core/BindingData.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				vk::CommandBuffer CommandBuffer::begin()
				{
					//Allocate
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(binding_data.commandPool, vk::CommandBufferLevel::ePrimary, 1);
					
					vk::CommandBuffer cmd;
					vk::Result const r = binding_data.device.allocateCommandBuffers(&alloc, &cmd);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate command buffer: " + to_string(r));
					
					//Begin
					vk::CommandBufferBeginInfo begin = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
					cmd.begin(&begin);
					return cmd;
				}

				void CommandBuffer::end(vk::CommandBuffer buffer)
				{
					//End
					buffer.end();

					//Submit
					vk::SubmitInfo submit = vk::SubmitInfo(0, nullptr, nullptr, 1, &buffer, 0, nullptr);
					binding_data.graphicsQueue.submit(1, &submit, nullptr);
					binding_data.graphicsQueue.waitIdle();
					
					//Free
					binding_data.device.freeCommandBuffers(binding_data.commandPool, 1, &buffer);
				}
			}
		}
	}
}
