#include "WindowContextVulkan.h"

#include "Misc/Console.h"

#include <stdint.h>
#include <vulkan/vulkan.hpp>

#include <Core/Rendering/Vulkan/WindowVulkan.h>
#include <Core/Rendering/Vulkan/HelperClasses/Swapchain.h>
#include "Core/Rendering/Vulkan/API/Extensions/ValidationLayers.h"
#include "Core/Rendering/Vulkan/API/Extensions/VulkanExtensions.h"
#include <GLFW/glfw3.h>
#include <set>
#include "Core/Rendering/Vulkan/HelperClasses/QueueFamilyIndices.h"
#include "Core/Rendering/Vulkan/HelperClasses/GPUVulkan.h"
#include <iostream>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				WindowContextVulkan::WindowContextVulkan(Vulkan::Window* window) : WindowContext(window)
				{
					initInstance();
					initDebugCallback();
					initWindowSurface();
					initGPU();
					initLogicalDevice();
					initSwapchain();
					initSemaphores();
				}

				WindowContextVulkan::~WindowContextVulkan()
				{
					device.destroySemaphore(imageAvailable);
					device.destroySemaphore(renderFinished);

					swapchain.reset();
					instance.destroySurfaceKHR(surface);
					device.destroy();
					debugEXT.reset();
					instance.destroy();
				}

				void WindowContextVulkan::prepareFrame()
				{
					//Request image
					const vk::Result r = device.acquireNextImageKHR(swapchain->getSwapchain(), INT64_MAX, imageAvailable, nullptr, &imgIndex);

					//Swapchain out of dte
					if (r == vk::Result::eErrorOutOfDateKHR || r == vk::Result::eSuboptimalKHR) //Swapchain out of date
						swapchain->rebuild(window->width, window->height);
					else if (r != vk::Result::eSuccess)
						Misc::Console::error("Failed to acquire swapchain image: " + to_string(r));
				}

				void WindowContextVulkan::finishFrame()
				{
					//Wait for the device to be finished submitting
					device.waitIdle();

					//Present
					vk::SwapchainKHR sc = swapchain->getSwapchain();
					vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinished, 1, &sc, &imgIndex, nullptr);
					presentQueue.presentKHR(&presentInfo);
				}

				void WindowContextVulkan::resize(int width, int height)
				{
					device.waitIdle();

					swapchain->rebuild(width, height);
				}

				void WindowContextVulkan::initInstance()
				{
					//Layers
					if (ValidationLayers::enabled() && !ValidationLayers::supported())
						Misc::Console::error("Validation layers requested but not available!");
					std::vector<const char*> extensions = VulkanExtensions::getRequiredExtensions();

					//AppInfo
					vk::ApplicationInfo appInfo = vk::ApplicationInfo("Tristeon", VK_MAKE_VERSION(2, 0, 0), "Tristeon", VK_MAKE_VERSION(2, 0, 0), VK_API_VERSION_1_0);

					//Instance Create Info
					const auto layerCount = ValidationLayers::enabled() ? ValidationLayers::validationLayers.size() : 0;
					const char* const* layerNames = ValidationLayers::enabled() ? ValidationLayers::validationLayers.data() : nullptr;
					vk::InstanceCreateInfo instanceCI = vk::InstanceCreateInfo({}, &appInfo, layerCount, layerNames, extensions.size(), extensions.data());

					//Create Instance
					const vk::Result r = vk::createInstance(&instanceCI, nullptr, &instance);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create Vulkan Instance. Error: " + to_string(r));
				}

				void WindowContextVulkan::initDebugCallback()
				{
					//Only if we are actually using validation layers
					if (!ValidationLayers::enabled())
						return;

					//Setup debug
					debugEXT = std::make_unique<DebugReportCallbackEXT>(instance);
					Misc::Console::t_assert(debugEXT->getResult() == vk::Result::eSuccess, "Failed to setup debug callback. Error: " + to_string(debugEXT->getResult()));
				}

				void WindowContextVulkan::initWindowSurface()
				{
					//Create window khr surface using GLFW's helper function
					VkSurfaceKHR psurf = VK_NULL_HANDLE;
					const VkResult r = glfwCreateWindowSurface(VkInstance(instance), window->window.get(), nullptr, &psurf);
					surface = vk::SurfaceKHR(psurf);
					Misc::Console::t_assert(r == VK_SUCCESS, "Failed to create window surface!");
				}

				void WindowContextVulkan::initGPU()
				{
					gpu = GPUVulkan::pickGPU(instance, surface);
				}

				void WindowContextVulkan::initLogicalDevice()
				{
					//Queue create info
					const QueueFamilyIndices indices = QueueFamilyIndices::get(gpu, surface);

					std::vector<vk::DeviceQueueCreateInfo> qcis;
					std::set<uint32_t> uniqueFamilies = { indices.graphicsFamily, indices.presentFamily };

					//Queues
					const float queuePriority = 1.0f;
					for (uint32_t queueFamily : uniqueFamilies)
					{
						const vk::DeviceQueueCreateInfo qci = vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
						qcis.push_back(qci);
					}

					//Set device info
					vk::PhysicalDeviceFeatures features = {};
					features.samplerAnisotropy = VK_TRUE;
					features.wideLines = VK_TRUE;

					//Layers
					const auto enabledLayerCount = ValidationLayers::validationLayers.size();
					const char * const * enabledLayers = ValidationLayers::validationLayers.data();

					//Extensions
					const auto extensionCount = VulkanExtensions::deviceExtensions.size();
					const char* const * enabledExtensions = VulkanExtensions::deviceExtensions.data();

					//Create
					vk::DeviceCreateInfo dci = vk::DeviceCreateInfo(vk::DeviceCreateFlags(), qcis.size(), qcis.data(), enabledLayerCount, enabledLayers, extensionCount, enabledExtensions, &features);
					const vk::Result r = gpu.createDevice(&dci, nullptr, &device);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create logical device!");

					//Receive queues
					graphicsQueue = device.getQueue(indices.graphicsFamily, 0);
					presentQueue = device.getQueue(indices.presentFamily, 0);
				}

				void WindowContextVulkan::initSwapchain()
				{
					swapchain = std::make_unique<Swapchain>(device, gpu, surface, window->width, window->height);
				}

				void WindowContextVulkan::initSemaphores()
				{
					//Create image semaphores
					vk::SemaphoreCreateInfo ci{};
					device.createSemaphore(&ci, nullptr, &imageAvailable);
					device.createSemaphore(&ci, nullptr, &renderFinished);
				}
			}
		}
	}
}
