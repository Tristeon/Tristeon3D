#include "RenderManager.h"

#include <set>
#include <vulkan/vulkan.hpp>

#include <Core/BindingData.h>
#include <Core/MessageBus.h>
#include <Core/Rendering/Helper/DeviceSelector.h>
#include <Core/Rendering/Helper/ValidationLayers.h>
#include <Misc/Console.h>

#include "Helper/Extensions.h"

namespace Tristeon::Core::Rendering
{
	RenderManager* RenderManager::instance = nullptr;

	RenderManager::RenderManager()
	{
		//Store instance
		instance = this;

		Misc::Console::write("[RENDERER] [INIT] Subscribing renderer to render & gameplay callbacks");
		
		MessageBus::subscribeToMessage(MT_RENDER, [&](Message msg) { render(); });
		MessageBus::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg) { inPlayMode = true; });
		MessageBus::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });
		MessageBus::subscribeToMessage(MT_WINDOW_RESIZE, [&](Message msg) { resized = true; });

		setup();
	}

	RenderManager::~RenderManager()
	{
		cleanSwapchain();
		
		binding_data.device.destroyCommandPool(binding_data.graphicsPool);
		binding_data.device.destroyCommandPool(binding_data.transferPool);

		binding_data.device.destroy();

		binding_data.instance.destroySurfaceKHR(binding_data.surface);

		if (binding_data.debug_messenger_enabled)
			ValidationLayers::destroyDebugUtilsMessenger(binding_data.instance, binding_data.debugMessenger);
		
		binding_data.instance.destroy();
	}

	void RenderManager::setup()
	{
		//Get extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		Extensions ext;
		for (int i = 0; i < glfwExtensionCount; ++i)
			ext.add_required(glfwExtensions[i]);

#if defined(TRISTEON_LOGENABLED)
		if (ValidationLayers::supported())
			binding_data.debug_messenger_enabled = ext.add_optional(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#else
		data.debug_messenger_enabled = false;
#endif
		auto extensions = ext.get();
		
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating vulkan instance");
		vk::ApplicationInfo app_info{ "Tristeon 3D", 1, "Tristeon", 1, 1 };
		const vk::InstanceCreateInfo instance_ci{ {}, &app_info,
			ValidationLayers::supported() ? (uint32_t)ValidationLayers::layers.size() : 0u, ValidationLayers::layers.data(),
			(uint32_t)extensions.size(), extensions.data()
		};
		binding_data.instance = createInstance(instance_ci);

		if (binding_data.debug_messenger_enabled)
		{
			Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating debug messenger");

			const vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
			const vk::DebugUtilsMessageTypeFlagsEXT messageTypes =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

			vk::DebugUtilsMessengerCreateInfoEXT messenger_ci{ {}, messageSeverity, messageTypes, ValidationLayers::debugCallback };
			VULKAN_DEBUG(ValidationLayers::createDebugUtilsMessenger(binding_data.instance, &messenger_ci, &binding_data.debugMessenger));
		}

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating window Surface");
		VkSurfaceKHR tempSurface;
		glfwCreateWindowSurface((VkInstance)binding_data.instance, binding_data.window, nullptr, &tempSurface);
		binding_data.surface = (vk::SurfaceKHR)tempSurface;
		
		binding_data.physical = DeviceSelector::select();

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Finding Queue families");
		binding_data.graphicsFamily = DeviceSelector::findGraphicsFamily();
		binding_data.presentFamily = DeviceSelector::findPresentFamily(binding_data.graphicsFamily);
		binding_data.transferFamily = DeviceSelector::findTransferFamily();

		std::set<uint32_t> families;
		families.insert(binding_data.graphicsFamily);
		families.insert(binding_data.presentFamily);
		families.insert(binding_data.transferFamily);

		std::vector<vk::DeviceQueueCreateInfo> queues;
		float priority = 1.0f;
		for (auto family : families)
			queues.push_back(vk::DeviceQueueCreateInfo{ {}, family, 1, &priority });

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating virtual device");
		auto features = binding_data.physical.getFeatures();
		vk::DeviceCreateInfo device_ci{ {}, (uint32_t)families.size(), queues.data(), 0, nullptr, DeviceSelector::gpuExtensions.size(), DeviceSelector::gpuExtensions.data(), &features };
		binding_data.device = binding_data.physical.createDevice(device_ci);
		
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Getting device queues");
		binding_data.graphicsQueue = binding_data.device.getQueue(binding_data.graphicsFamily, 0);
		binding_data.presentQueue = binding_data.device.getQueue(binding_data.presentFamily, binding_data.graphicsFamily == binding_data.presentFamily ? 0 : 1);
		binding_data.transferQueue = binding_data.device.getQueue(binding_data.transferFamily, families.size() - 1);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating command pool for graphics");
		vk::CommandPoolCreateInfo graphics_pool_ci{ {}, binding_data.graphicsFamily };
		binding_data.graphicsPool = binding_data.device.createCommandPool(graphics_pool_ci);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating command pool for transfer operations");
		vk::CommandPoolCreateInfo transfer_pool_ci{ {}, binding_data.transferFamily };
		binding_data.transferPool = binding_data.device.createCommandPool(transfer_pool_ci);

		buildSwapchain();
		
		vk::SemaphoreCreateInfo sema_ci{ };
		binding_data.semaImageAvailable = binding_data.device.createSemaphore(sema_ci);
		binding_data.semaRenderFinished = binding_data.device.createSemaphore(sema_ci);
	}

	void RenderManager::render()
	{
		while (!glfwWindowShouldClose(binding_data.window)) {
			glfwPollEvents();

			if (resized)
			{
				resized = false;
				buildSwapchain();
			}

			const auto result = binding_data.device.acquireNextImageKHR(binding_data.swapchain, UINT64_MAX, binding_data.semaImageAvailable, nullptr);
			uint32_t index = result.value;
			if (result.result == vk::Result::eErrorOutOfDateKHR)
			{
				resized = true;
				continue;
			}

			//Submit main cmd buffer
			vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eTopOfPipe;
			vk::SubmitInfo submit{
				1, &binding_data.semaImageAvailable,
				&stage,
				1, &binding_data.outputCommandBuffers[index],
				1, &binding_data.semaRenderFinished
			};
			
			binding_data.graphicsQueue.submit(submit, nullptr);

			//Present the image to the screen
			vk::PresentInfoKHR present{ 1, &binding_data.semaRenderFinished, 1, &binding_data.swapchain, &index };
			{
				const vk::Result result = binding_data.presentQueue.presentKHR(present);
				if (result == vk::Result::eErrorOutOfDateKHR)
					resized = true;
				else if (result != vk::Result::eSuccess)
					Misc::Console::write("[RENDERER] [ERROR] [VULKAN] [CODE: " + vk::to_string(result) + "] " + "data.presentQueue.presentKHR(present)");
			}

			binding_data.device.waitIdle();
		}

		binding_data.device.waitIdle();
	}

	void RenderManager::buildSwapchain()
	{
		cleanSwapchain();

		binding_data.swapchain = DeviceSelector::swapchain();
		binding_data.swapchainImages = binding_data.device.getSwapchainImagesKHR(binding_data.swapchain);
		binding_data.swapchainImageViews.resize(binding_data.swapchainImages.size());
		for (auto i = 0; i < binding_data.swapchainImageViews.size(); i++)
		{
			vk::ImageViewCreateInfo ci{ {}, binding_data.swapchainImages[i], vk::ImageViewType::e2D,
				binding_data.format.format, {},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
			};
			binding_data.swapchainImageViews[i] = binding_data.device.createImageView(ci);
		}

		//Main renderpass
		vk::AttachmentDescription color_output{ {}, binding_data.format.format, vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
		};
		vk::AttachmentReference color_output_ref{ 0, vk::ImageLayout::eColorAttachmentOptimal };

		vk::SubpassDescription subpass{ {}, vk::PipelineBindPoint::eGraphics,
			0, nullptr,
			1, &color_output_ref,
			nullptr, nullptr,
			0, nullptr
		};

		const vk::RenderPassCreateInfo render_pass_ci{ {}, 1, &color_output, 1, &subpass, 0, nullptr };
		binding_data.outputPass = binding_data.device.createRenderPass(render_pass_ci);

		//Create framebuffers for the screen output
		binding_data.swapchainFramebuffers.resize(binding_data.swapchainImages.size());
		for (auto i = 0; i < binding_data.swapchainFramebuffers.size(); i++)
		{
			std::array<vk::ImageView, 1> attachments{ binding_data.swapchainImageViews[i] };
			vk::FramebufferCreateInfo ci{ {}, binding_data.outputPass,
				attachments.size(), attachments.data(),
				binding_data.extent.width, binding_data.extent.height, 1
			};
			binding_data.swapchainFramebuffers[i] = binding_data.device.createFramebuffer(ci);
		}

		ShaderFile shader{ "Files/Shaders/ColorV.spv", "Files/Shaders/ColorF.spv" };
		screenMat = std::make_unique<Material>(shader, PipelineProperties{});
		screenMat->buildPipeline();

		//Command buffers
		vk::CommandBufferAllocateInfo cmd_info{ binding_data.graphicsPool, vk::CommandBufferLevel::ePrimary, (uint32_t)binding_data.swapchainImages.size() };
		binding_data.outputCommandBuffers = binding_data.device.allocateCommandBuffers(cmd_info);

		for (int i = 0; i < binding_data.outputCommandBuffers.size(); i++)
		{
			vk::CommandBufferBeginInfo buffer_begin{ {}, nullptr };
			binding_data.outputCommandBuffers[i].begin(buffer_begin);
			{
				vk::ClearValue clear = vk::ClearValue{};
				clear.color = vk::ClearColorValue{ std::array<float, 4> { 0, 0, 1, 1 } };

				vk::RenderPassBeginInfo pass_begin{
					binding_data.outputPass,
					binding_data.swapchainFramebuffers[i],
					vk::Rect2D { vk::Offset2D { 0, 0 }, binding_data.extent },
					1,
					&clear
				};

				binding_data.outputCommandBuffers[i].beginRenderPass(pass_begin, vk::SubpassContents::eInline);
				{
					binding_data.outputCommandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, screenMat->pipeline());
					binding_data.outputCommandBuffers[i].draw(3, 1, 0, 0);
				}
				binding_data.outputCommandBuffers[i].endRenderPass();

			}
			binding_data.outputCommandBuffers[i].end();
		}
	}

	void RenderManager::cleanSwapchain()
	{
		if (!binding_data.swapchain)
			return;

		for (auto fb : binding_data.swapchainFramebuffers)
			binding_data.device.destroyFramebuffer(fb);

		binding_data.device.freeCommandBuffers(binding_data.graphicsPool, (uint32_t)binding_data.outputCommandBuffers.size(), binding_data.outputCommandBuffers.data());
		
		binding_data.device.destroyRenderPass(binding_data.outputPass);

		for (auto view : binding_data.swapchainImageViews)
			binding_data.device.destroyImageView(view);
		
		binding_data.device.destroySwapchainKHR(binding_data.swapchain);
	}
}
