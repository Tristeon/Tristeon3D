#include "OneTimeCmd.h"
#include <vulkan/vulkan.hpp>

#include "Core/RenderData.h"
#include "Core/Engine.h"
#include "Misc/Console.h"

namespace Tristeon::Core::Rendering
{
	vk::CommandBuffer OneTimeCmd::begin(const vk::CommandPool& commandPool)
	{
		//Allocate
		const auto alloc = vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
		const auto cmd = renderData.device.allocateCommandBuffers(alloc)[0];

		//Begin
		auto begin = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		VULKAN_DEBUG(cmd.begin(&begin));
		return cmd;
	}

	void OneTimeCmd::end(const vk::CommandBuffer& buffer, const vk::Queue& queue, const vk::CommandPool& pool)
	{
		buffer.end();

		auto submit = vk::SubmitInfo(0, nullptr, nullptr, 1, &buffer, 0, nullptr);
		VULKAN_DEBUG(queue.submit(1, &submit, nullptr));

		queue.waitIdle(); //TODO: Reduce wait times by only waiting when the resources are needed
		renderData.device.freeCommandBuffers(pool, 1, &buffer); 
	}
}