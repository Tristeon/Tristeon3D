#include "RenderManager.h"

#include <set>
#include <vulkan/vulkan.hpp>

#include <Core/BindingData.h>
#include <Core/MessageBus.h>
#include <Core/Rendering/Helper/DeviceSelector.h>
#include <Core/Rendering/Helper/ValidationLayers.h>
#include <Misc/Console.h>



#include "Core/Collector.h"
#include "Core/SceneManager.h"
#include "Helper/Extensions.h"
#include "Materials/ScreenMaterial.h"

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
		binding_data.semaOutputFinished = binding_data.device.createSemaphore(sema_ci);
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

			//Start acquiring the next image, this might sometimes take a moment so this will be a good time to start our deferred pass too
			const auto result = binding_data.device.acquireNextImageKHR(binding_data.swapchain, UINT64_MAX, binding_data.semaImageAvailable, nullptr);
			uint32_t index = result.value;
			if (result.result == vk::Result::eErrorOutOfDateKHR)
			{
				resized = true;
				continue;
			}

			//Render deferred pass
			//SceneManager::current()->recordSceneCmd();
			//vk::PipelineStageFlags deferred_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			//vk::SubmitInfo deferred_submit{
			//	1, &binding_data.semaImageAvailable,
			//	&deferred_stage,
			//	1, &binding_data.offscreenBuffer,
			//	1, &binding_data.semaOffscreenFinished
			//};
			//binding_data.graphicsQueue.submit(deferred_submit);

			//Submit output cmd buffer
			std::array<vk::Semaphore, 1> output_wait_semaphores{ binding_data.semaOffscreenFinished };
			vk::PipelineStageFlags output_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			vk::SubmitInfo output_submit{
				(uint32_t)output_wait_semaphores.size(), output_wait_semaphores.data(),
				&output_stage,
				1, &binding_data.outputCommandBuffers[index],
				1, &binding_data.semaOutputFinished
			};

			binding_data.graphicsQueue.submit(output_submit);

			//Present the image to the screen
			vk::PresentInfoKHR present{ 1, &binding_data.semaOutputFinished, 1, &binding_data.swapchain, &index };
			{
				const vk::Result result = binding_data.presentQueue.presentKHR(present);
				if (result == vk::Result::eErrorOutOfDateKHR)
					resized = true;
				else if (result != vk::Result::eSuccess)
					Misc::Console::write("[RENDERER] [ERROR] [VULKAN] [CODE: " + vk::to_string(result) + "] " + "data.presentQueue.presentKHR(present)");
			}
		}

		binding_data.device.waitIdle();
	}

	void RenderManager::buildSwapchain()
	{
		cleanSwapchain();

		binding_data.swapchain = DeviceSelector::swapchain();
		binding_data.swapchainImages = binding_data.device.getSwapchainImagesKHR(binding_data.swapchain);
		binding_data.swapchainImageViews.resize(binding_data.swapchainImages.size());
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating Swapchain image views");
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

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating output render pass");
		const vk::RenderPassCreateInfo render_pass_ci{ {}, 1, &color_output, 1, &subpass, 0, nullptr };
		binding_data.outputPass = binding_data.device.createRenderPass(render_pass_ci);

		//Create framebuffers for the screen output
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating swapchain frame buffers");
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

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating output material");
		screenMat = std::make_unique<ScreenMaterial>(PipelineProperties{});

		for (auto mat : Collector<Material>::all())
			mat->buildPipeline();

		//Output command buffers
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Recording output command buffers");
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

		//Deferred
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating deferred command buffer & semaphore");
		vk::CommandBufferAllocateInfo alloc{ binding_data.graphicsPool, vk::CommandBufferLevel::ePrimary, 1 };
		binding_data.offscreenBuffer = binding_data.device.allocateCommandBuffers(alloc)[0];
		binding_data.semaOffscreenFinished = binding_data.device.createSemaphore({});

		//Deferred render pass
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating deferred render pass with 3 output attachments");
		std::array<vk::AttachmentDescription, 3> deferred_descriptions = {
			vk::AttachmentDescription { {}, binding_data.format.format, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			},
			vk::AttachmentDescription { {}, vk::Format::eD32Sfloat, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			},
			vk::AttachmentDescription { {}, binding_data.format.format, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			}
		};

		std::array<vk::AttachmentReference, 3> deferred_color_refs = {
			vk::AttachmentReference { 0, vk::ImageLayout::eColorAttachmentOptimal },
			vk::AttachmentReference { 2, vk::ImageLayout::eColorAttachmentOptimal}
		};
		vk::AttachmentReference deferred_depth_ref = vk::AttachmentReference{ 1, vk::ImageLayout::eDepthAttachmentOptimal };

		vk::SubpassDescription deferred_subpass{ {}, vk::PipelineBindPoint::eGraphics,
			0, nullptr,
			(uint32_t)deferred_color_refs.size(), deferred_color_refs.data(),
			nullptr,
			&deferred_depth_ref,
			0, nullptr
		};

		vk::RenderPassCreateInfo deferred_rp_ci{ {}, (uint32_t)deferred_descriptions.size(), deferred_descriptions.data(), 1, &deferred_subpass, 0, nullptr };
		binding_data.offscreenPass = binding_data.device.createRenderPass(deferred_rp_ci);

		//Deferred frame buffer
		binding_data.offscreenColor = createAttachment(binding_data.format.format, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eColorAttachmentOptimal);
		binding_data.offscreenDepth = createAttachment(vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, vk::ImageLayout::eDepthAttachmentOptimal);
		binding_data.offscreenNormal = createAttachment(binding_data.format.format, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eColorAttachmentOptimal);

		std::array<vk::ImageView, 3> deferredViews{ binding_data.offscreenColor.view, binding_data.offscreenDepth.view, binding_data.offscreenNormal.view };

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating deferred framebuffer");
		vk::FramebufferCreateInfo deferred_fb_ci{ {}, binding_data.offscreenPass, deferredViews, binding_data.extent.width, binding_data.extent.height, 1 };
		binding_data.offscreenFramebuffer = binding_data.device.createFramebuffer(deferred_fb_ci);
	}

	void RenderManager::cleanSwapchain()
	{
		if (!binding_data.swapchain)
			return;

		binding_data.device.destroyFramebuffer(binding_data.offscreenFramebuffer);

		binding_data.device.destroyImage(binding_data.offscreenColor.image);
		binding_data.device.destroyImage(binding_data.offscreenNormal.image);
		binding_data.device.destroyImage(binding_data.offscreenDepth.image);

		binding_data.device.destroyImageView(binding_data.offscreenColor.view);
		binding_data.device.destroyImageView(binding_data.offscreenNormal.view);
		binding_data.device.destroyImageView(binding_data.offscreenDepth.view);

		binding_data.device.freeMemory(binding_data.offscreenColor.memory);
		binding_data.device.freeMemory(binding_data.offscreenNormal.memory);
		binding_data.device.freeMemory(binding_data.offscreenDepth.memory);

		binding_data.device.destroyRenderPass(binding_data.offscreenPass);

		binding_data.device.freeCommandBuffers(binding_data.graphicsPool, 1, &binding_data.offscreenBuffer);
		binding_data.device.destroySemaphore(binding_data.semaOffscreenFinished);

		screenMat.reset();

		for (auto fb : binding_data.swapchainFramebuffers)
			binding_data.device.destroyFramebuffer(fb);

		binding_data.device.freeCommandBuffers(binding_data.graphicsPool, (uint32_t)binding_data.outputCommandBuffers.size(), binding_data.outputCommandBuffers.data());

		binding_data.device.destroyRenderPass(binding_data.outputPass);

		for (auto view : binding_data.swapchainImageViews)
			binding_data.device.destroyImageView(view);

		binding_data.device.destroySwapchainKHR(binding_data.swapchain);
	}

	FrameBufferAttachment RenderManager::createAttachment(vk::Format format, vk::ImageAspectFlags aspect, vk::ImageLayout layout)
	{
		FrameBufferAttachment result;

		std::array<uint32_t, 3> families{ binding_data.transferFamily, binding_data.graphicsFamily, binding_data.presentFamily };

		const vk::ImageCreateInfo image_ci{ {}, vk::ImageType::e2D, format,
			vk::Extent3D{ binding_data.extent, 1 }, 1, 1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled, vk::SharingMode::eConcurrent,
			families.size(), families.data() , layout
		};
		result.image = binding_data.device.createImage(image_ci);

		const auto requirements = binding_data.device.getImageMemoryRequirements(result.image);
		const vk::MemoryAllocateInfo memory_info{ requirements.size, DeviceSelector::getMemoryType(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal) };
		result.memory = binding_data.device.allocateMemory(memory_info);
		binding_data.device.bindImageMemory(result.image, result.memory, 0);

		const vk::ImageViewCreateInfo view_ci{ {}, result.image, vk::ImageViewType::e2D, format, vk::ComponentMapping{}, vk::ImageSubresourceRange { aspect, 1, 1, 0, 1 } };
		result.view = binding_data.device.createImageView(view_ci);
		return result;
	}
}