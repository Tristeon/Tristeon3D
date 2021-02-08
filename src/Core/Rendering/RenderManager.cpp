#include "RenderManager.h"

#include <set>
#include <vulkan/vulkan.hpp>

#include <Core/BindingData.h>
#include <Core/MessageBus.h>
#include <Core/Rendering/Helper/DeviceSelector.h>
#include <Core/Rendering/Helper/ValidationLayers.h>
#include <Misc/Console.h>

namespace Tristeon::Core::Rendering
{
	RenderManager* RenderManager::instance = nullptr;

	RenderManager::RenderManager()
	{
		//Store instance
		instance = this;

		Misc::Console::write("[RENDERER] [INIT] Susbscribing renderer to render & gameplay callbacks");
		
		MessageBus::subscribeToMessage(MT_RENDER, [&](Message msg) { render(); });
		MessageBus::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg) { inPlayMode = true; });
		MessageBus::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });

		setup();
	}

	RenderManager::~RenderManager()
	{
		cleanSwapchain();
		
		binding_data.device.destroyCommandPool(binding_data.graphicsPool);
		binding_data.device.destroyCommandPool(binding_data.transferPool);

		binding_data.device.destroy();

		binding_data.instance.destroySurfaceKHR(binding_data.surface);
		binding_data.instance.destroy();
	}

	void RenderManager::setup()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating vulkan instance");
		vk::ApplicationInfo app_info{ "Tristeon 3D", 1, "Tristeon", 1, 1 };
		const vk::InstanceCreateInfo instance_ci{ {}, &app_info, ValidationLayers::supported() ? ValidationLayers::layers.size() : 0, ValidationLayers::layers.data(), 0, nullptr };
		binding_data.instance = createInstance(instance_ci);

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
	}

	void RenderManager::render()
	{
		
	}

	void RenderManager::buildSwapchain()
	{
		cleanSwapchain();

		binding_data.swapchain = DeviceSelector::swapchain();
		binding_data.swapchain_images = binding_data.device.getSwapchainImagesKHR(binding_data.swapchain);
		binding_data.swapchain_image_views.resize(binding_data.swapchain_images.size());
		for (auto i = 0; i < binding_data.swapchain_image_views.size(); i++)
		{
			vk::ImageViewCreateInfo ci{ {}, binding_data.swapchain_images[i], vk::ImageViewType::e2D,
				binding_data.surface_format.format, {},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
			};
			binding_data.swapchain_image_views[i] = binding_data.device.createImageView(ci);
		}

		//Main renderpass
		vk::AttachmentDescription color_output{ {}, binding_data.surface_format.format, vk::SampleCountFlagBits::e1,
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
		binding_data.output_pass = binding_data.device.createRenderPass(render_pass_ci);

		//Create framebuffers for the screen output
		binding_data.swapchain_framebuffers.resize(binding_data.swapchain_images.size());
		for (auto i = 0; i < binding_data.swapchain_framebuffers.size(); i++)
		{
			std::array<vk::ImageView, 1> attachments{ binding_data.swapchain_image_views[i] };
			vk::FramebufferCreateInfo ci{ {}, binding_data.output_pass,
				attachments.size(), attachments.data(),
				binding_data.extent.width, binding_data.extent.height, 1
			};
			binding_data.swapchain_framebuffers[i] = binding_data.device.createFramebuffer(ci);
		}
	}

	void RenderManager::cleanSwapchain()
	{
		if (!binding_data.swapchain)
			return;
		
		binding_data.device.destroyRenderPass(binding_data.output_pass);
		binding_data.device.destroySwapchainKHR(binding_data.swapchain);
	}
}