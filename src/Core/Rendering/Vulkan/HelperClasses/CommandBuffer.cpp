#include "CommandBuffer.h"
#include <vulkan/vulkan.hpp>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				vk::CommandBuffer CommandBuffer::begin(vk::CommandPool commandPool, vk::Device device)
				{
					//Allocate
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
					
					vk::CommandBuffer cmd;
					vk::Result const r = device.allocateCommandBuffers(&alloc, &cmd);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate command buffer: " + to_string(r));
					
					//Begin
					vk::CommandBufferBeginInfo begin = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
					cmd.begin(&begin);
					return cmd;
				}

				void CommandBuffer::end(vk::CommandBuffer buffer, vk::Queue graphicsQueue, vk::Device device, vk::CommandPool cmdPool)
				{
					//End
					buffer.end();

					//Submit
					vk::SubmitInfo submit = vk::SubmitInfo(0, nullptr, nullptr, 1, &buffer, 0, nullptr);
					graphicsQueue.submit(1, &submit, nullptr);
					graphicsQueue.waitIdle();
					
					//Free
					device.freeCommandBuffers(cmdPool, 1, &buffer);
				}
			}
		}
	}
}
