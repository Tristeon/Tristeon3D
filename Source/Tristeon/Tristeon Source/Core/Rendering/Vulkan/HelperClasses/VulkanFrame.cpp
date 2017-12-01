#include "VulkanFrame.h"
#include "Misc/Console.h"
#include <vulkan/vulkan.hpp>

int Tristeon::Core::Rendering::Vulkan::VulkanFrame::prepare(vk::Device device, vk::SwapchainKHR swapchain, vk::Semaphore imageAvailable)
{
	//Request image
	uint32_t index;
	const vk::Result r = device.acquireNextImageKHR(swapchain, INT64_MAX, imageAvailable, nullptr, &index);

	//Swapchain out of dte
	if (r == vk::Result::eErrorOutOfDateKHR || r == vk::Result::eSuboptimalKHR) //Swapchain out of date
		Misc::Console::error("Swapchain recreation has yet to be implemented!");
	else if (r != vk::Result::eSuccess)
		Misc::Console::error("Failed to acquire swapchain image: " + to_string(r));
	return index;
}

void Tristeon::Core::Rendering::Vulkan::VulkanFrame::submit(vk::Device device, vk::SwapchainKHR swapchain, vk::Semaphore renderFinished, vk::Queue presentQueue, const uint32_t index)
{
	//Wait for the device to be finished submitting
	device.waitIdle();

	//Present
	vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinished, 1, &swapchain, &index, nullptr);
	presentQueue.presentKHR(&presentInfo);
}
