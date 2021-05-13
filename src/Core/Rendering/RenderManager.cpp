#include "RenderManager.h"

#include <set>
#include <vulkan/vulkan.hpp>

#include <Core/RenderData.h>
#include <Core/MessageBus.h>
#include <Core/Rendering/Helper/DeviceSelector.h>
#include <Core/Rendering/Helper/ValidationLayers.h>
#include <Misc/Console.h>

#include "Core/Collector.h"
#include "Core/SceneManager.h"
#include "Core/Components/Camera.h"
#include "Data/Resources.h"
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

		MessageBus::subscribe(Message::Type::Render, [&](Message msg) { render(); });
		MessageBus::subscribe(Message::Type::GameLogicStart, [&](Message msg) { inPlayMode = true; });
		MessageBus::subscribe(Message::Type::GameLogicStop, [&](Message msg) { inPlayMode = false; });
		MessageBus::subscribe(Message::Type::WindowResize, [&](Message msg) { shouldRebuildSwapchain = true; });
		MessageBus::subscribe(Message::Type::Quitting, [&](Message msg) { renderData.device.waitIdle(); });

		setup();
	}

	RenderManager::~RenderManager()
	{
		const auto d = renderData.device;

		for (auto& frame : renderData.frame)
		{
			d.destroySemaphore(frame.imageAvailable);
			d.destroySemaphore(frame.offscreenFinished);
			d.destroySemaphore(frame.onscreenFinished);

			d.destroyFence(frame.fence);
		}

		d.destroyDescriptorSetLayout(renderData.transformLayout);

		cleanSwapchain();

		screenMat.reset();

		for (auto* shader : Collector<ShaderFile>::all())
			shader->clean();

		d.destroyDescriptorPool(renderData.descriptorPool);

		d.destroyCommandPool(renderData.graphics.pool);
		d.destroyCommandPool(renderData.present.pool);
		d.destroyCommandPool(renderData.transfer.pool);

		d.destroy();

		renderData.instance.destroySurfaceKHR(renderData.surface);

		if (renderData.debugMessengerEnabled)
			ValidationLayers::destroyDebugUtilsMessenger(renderData.instance, renderData.debugMessenger);

		renderData.instance.destroy();
	}

	void RenderManager::setup()
	{
		const auto extensions = getInstanceExtensions();
		createInstance(extensions);
		createDebugMessenger();

		createSurface();
		selectPhysicalDevice();
		const auto families = selectQueueFamilies();
		createVirtualDevice(families);

		createDescriptorPool();
		createCommandPools();

		createOnscreenMaterial();

		createFrameData();

		createOffscreenTransformLayout();

		buildSwapchain();
	}

	void RenderManager::render()
	{
		if (shouldRebuildSwapchain)
		{
			shouldRebuildSwapchain = false;
			buildSwapchain();
		}

		//Get next swapchain image
		const auto result = renderData.device.acquireNextImageKHR(renderData.swapchain, UINT64_MAX, renderData.frame[currentFrame].imageAvailable, nullptr);
		const auto index = result.value;
		if (result.result == vk::Result::eErrorOutOfDateKHR)
		{
			shouldRebuildSwapchain = true;
			return;
		}

		const auto wait = drawOffscreen();
		drawOnscreen(wait, index);

		currentFrame = (currentFrame + 1) % RenderData::FRAMES_IN_FLIGHT;
	}

	std::vector<vk::Semaphore> RenderManager::drawOffscreen()
	{
		//If we looped around too quick, wait for the current frame's fence to be completed
		VULKAN_DEBUG(renderData.device.waitForFences(renderData.frame[currentFrame].fence, true, UINT64_MAX));
		renderData.device.resetFences(renderData.frame[currentFrame].fence);

		//Render deferred pass
		std::vector<vk::Semaphore> waitSemaphores;

		SceneManager::current()->recordSceneCmd(renderData.frame[currentFrame].offscreen, currentFrame, renderData.frame[currentFrame].offscreenFramebuffer);
		vk::PipelineStageFlags deferred_stage = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::SubmitInfo deferred_submit{
			1, &renderData.frame[currentFrame].imageAvailable,
			&deferred_stage,
			1, &renderData.frame[currentFrame].offscreen,
			1, &renderData.frame[currentFrame].offscreenFinished
		};
		renderData.graphics.queue.submit(deferred_submit);
		waitSemaphores.push_back(renderData.frame[currentFrame].offscreenFinished);

		if (!Collector<Components::Camera>::all().empty())
			waitSemaphores.push_back(renderData.frame[currentFrame].imageAvailable);

		return waitSemaphores;
	}

	void RenderManager::drawOnscreen(std::vector<vk::Semaphore> wait, const uint32_t& swapchainIndex)
	{
		//Submit output cmd buffer
		vk::PipelineStageFlags outputStage = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::SubmitInfo outputSubmit{
			(uint32_t)wait.size(), wait.data(),
			&outputStage,
			1, &renderData.onscreenCmds[swapchainIndex],
			1, &renderData.frame[currentFrame].onscreenFinished
		};

		renderData.graphics.queue.submit(outputSubmit, renderData.frame[currentFrame].fence);

		//Present the image to the screen
		const vk::PresentInfoKHR present{ 1, &renderData.frame[currentFrame].onscreenFinished, 1, &renderData.swapchain, &swapchainIndex };
		{
			const auto result = renderData.present.queue.presentKHR(present);
			if (result == vk::Result::eErrorOutOfDateKHR)
				shouldRebuildSwapchain = true;
			else if (result != vk::Result::eSuccess)
				Misc::Console::write("[RENDERER] [ERROR] [VULKAN] [CODE: " + vk::to_string(result) + "] " + "data.presentQueue.presentKHR(present)");
		}
	}

	std::vector<const char*> RenderManager::getInstanceExtensions()
	{
		//Get extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		Extensions ext;
		for (int i = 0; i < glfwExtensionCount; ++i)
			ext.add_required(glfwExtensions[i]);

#if defined(TRISTEON_LOGENABLED)
		if (ValidationLayers::supported())
			renderData.debugMessengerEnabled = ext.add_optional(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#else
		data.debug_messenger_enabled = false;
#endif
		return ext.get();
	}

	void RenderManager::createInstance(const std::vector<const char*>& extensions)
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating vulkan instance");
		vk::ApplicationInfo app_info{ "Tristeon 3D", 1, "Tristeon", 1, 1 };
		const vk::InstanceCreateInfo instance_ci{ {}, &app_info,
			ValidationLayers::supported() ? (uint32_t)ValidationLayers::layers.size() : 0u, ValidationLayers::layers.data(),
			(uint32_t)extensions.size(), extensions.data()
		};
		renderData.instance = vk::createInstance(instance_ci);
	}

	void RenderManager::createDebugMessenger()
	{
		if (renderData.debugMessengerEnabled)
		{
			Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating debug messenger");

			const vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
			const vk::DebugUtilsMessageTypeFlagsEXT messageTypes =
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

			vk::DebugUtilsMessengerCreateInfoEXT messenger_ci{ {}, messageSeverity, messageTypes, ValidationLayers::debugCallback };
			VULKAN_DEBUG(ValidationLayers::createDebugUtilsMessenger(renderData.instance, &messenger_ci, &renderData.debugMessenger));
		}
	}

	void RenderManager::createSurface()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating window Surface");
		VkSurfaceKHR tempSurface;
		glfwCreateWindowSurface((VkInstance)renderData.instance, renderData.window, nullptr, &tempSurface);
		renderData.surface = (vk::SurfaceKHR)tempSurface;
	}

	void RenderManager::selectPhysicalDevice()
	{
		renderData.physical = DeviceSelector::select();
	}

	std::set<uint32_t> RenderManager::selectQueueFamilies()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Finding Queue families");
		renderData.graphics.family = DeviceSelector::findGraphicsFamily();
		renderData.present.family = DeviceSelector::findPresentFamily(renderData.graphics.family);
		renderData.transfer.family = DeviceSelector::findTransferFamily();
		renderData.compute.family = DeviceSelector::findComputeFamily();

		Misc::Console::write("\tGraphics Family: " + std::to_string(renderData.graphics.family));
		Misc::Console::write("\tPresent Family: " + std::to_string(renderData.present.family));
		Misc::Console::write("\tTransfer Family: " + std::to_string(renderData.transfer.family));
		Misc::Console::write("\tCompute Family: " + std::to_string(renderData.compute.family));

		std::set<uint32_t> families;
		families.insert(renderData.graphics.family);
		families.insert(renderData.present.family);
		families.insert(renderData.transfer.family);
		families.insert(renderData.compute.family);
		return families;
	}

	void RenderManager::createVirtualDevice(const std::set<uint32_t>& families)
	{
		std::vector<vk::DeviceQueueCreateInfo> queues;
		float priority = 1.0f;
		for (auto family : families)
			queues.push_back(vk::DeviceQueueCreateInfo{ {}, family, 1, &priority });

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating virtual device");
		auto features = renderData.physical.getFeatures();
		vk::DeviceCreateInfo device_ci{ {}, (uint32_t)families.size(), queues.data(), 0, nullptr, DeviceSelector::gpuExtensions.size(), DeviceSelector::gpuExtensions.data(), &features };
		renderData.device = renderData.physical.createDevice(device_ci);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Getting device queues");
		renderData.graphics.queue = renderData.device.getQueue(renderData.graphics.family, 0);
		renderData.present.queue = renderData.device.getQueue(renderData.present.family, 0);
		renderData.transfer.queue = renderData.device.getQueue(renderData.transfer.family, 0);
		renderData.compute.queue = renderData.device.getQueue(renderData.compute.family, 0);
	}

	void RenderManager::createDescriptorPool()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating descriptor pool");
		std::array<vk::DescriptorPoolSize, 2> descriptor_pool_sizes{
			vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, 1 },
			vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, 10 }
		};
		const vk::DescriptorPoolCreateInfo descriptor_ci{ vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 50, descriptor_pool_sizes };
		renderData.descriptorPool = renderData.device.createDescriptorPool(descriptor_ci);
	}

	void RenderManager::createCommandPools()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating command pool for graphics");
		const vk::CommandPoolCreateInfo graphics_pool_ci{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer, renderData.graphics.family };
		renderData.graphics.pool = renderData.device.createCommandPool(graphics_pool_ci);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating command pool for transfer operations");
		const vk::CommandPoolCreateInfo transfer_pool_ci{ {}, renderData.transfer.family };
		renderData.transfer.pool = renderData.device.createCommandPool(transfer_pool_ci);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating command pool for compute operations");
		const vk::CommandPoolCreateInfo compute_pool_ci{ {}, renderData.compute.family };
		renderData.compute.pool = renderData.device.createCommandPool(compute_pool_ci);
	}

	void RenderManager::createOnscreenMaterial()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating output material");
		screenMat = std::make_unique<ScreenMaterial>(PipelineProperties{});
	}

	void RenderManager::createFrameData()
	{
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating primary command buffers");
		const vk::CommandBufferAllocateInfo alloc{ renderData.graphics.pool, vk::CommandBufferLevel::ePrimary, RenderData::FRAMES_IN_FLIGHT };
		auto buffers = renderData.device.allocateCommandBuffers(alloc);

		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating frame semaphores");
		for (uint32_t i = 0; i < RenderData::FRAMES_IN_FLIGHT; i++)
		{
			renderData.frame[i].imageAvailable = renderData.device.createSemaphore({});
			renderData.frame[i].offscreenFinished = renderData.device.createSemaphore({});
			renderData.frame[i].onscreenFinished = renderData.device.createSemaphore({});

			renderData.frame[i].offscreen = buffers[i];

			renderData.frame[i].fence = renderData.device.createFence({ vk::FenceCreateFlagBits::eSignaled });
		}
	}

	void RenderManager::createOffscreenTransformLayout()
	{
		vk::DescriptorSetLayoutBinding ubo{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr };
		renderData.transformLayout = renderData.device.createDescriptorSetLayout({ {}, ubo });
	}

	void RenderManager::buildSwapchain() const
	{
		cleanSwapchain();

		renderData.swapchain = DeviceSelector::swapchain();
		renderData.swapchainImages = renderData.device.getSwapchainImagesKHR(renderData.swapchain);

		createOnscreenImageViews();
		createOnscreenRenderpass();
		createOnscreenFramebuffers();

		createOffscreenRenderPass();
		createOffscreenFramebuffer();

		prepareMaterials();

		recordOnscreenCommandBuffers();
	}

	void RenderManager::cleanSwapchain()
	{
		if (!renderData.swapchain)
			return;

		for (auto frame : renderData.frame)
		{
			renderData.device.destroyFramebuffer(frame.offscreenFramebuffer);
			frame.offscreenColor.destroy(renderData.device);
			frame.offscreenDepth.destroy(renderData.device);
			frame.offscreenNormal.destroy(renderData.device);

			renderData.device.freeCommandBuffers(renderData.graphics.pool, 1, &frame.offscreen);
		}

		renderData.device.destroyRenderPass(renderData.offscreenPass);

		for (auto fb : renderData.swapchainFramebuffers)
			renderData.device.destroyFramebuffer(fb);

		renderData.device.destroyRenderPass(renderData.outputPass);

		for (auto view : renderData.swapchainImageViews)
			renderData.device.destroyImageView(view);

		renderData.device.destroySwapchainKHR(renderData.swapchain);
	}

	void RenderManager::createOnscreenImageViews()
	{
		renderData.swapchainImageViews.resize(renderData.swapchainImages.size());
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating Swapchain image views");
		for (auto i = 0; i < renderData.swapchainImageViews.size(); i++)
		{
			vk::ImageViewCreateInfo ci{ {}, renderData.swapchainImages[i], vk::ImageViewType::e2D,
				renderData.format.format, {},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
			};
			renderData.swapchainImageViews[i] = renderData.device.createImageView(ci);
		}
	}

	void RenderManager::createOnscreenRenderpass()
	{
		//Main renderpass
		vk::AttachmentDescription color_output{ {}, renderData.format.format, vk::SampleCountFlagBits::e1,
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
		renderData.outputPass = renderData.device.createRenderPass(render_pass_ci);
	}

	void RenderManager::createOnscreenFramebuffers()
	{
		//Create framebuffers for the screen output
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating swapchain frame buffers");
		renderData.swapchainFramebuffers.resize(renderData.swapchainImages.size());
		for (auto i = 0; i < renderData.swapchainFramebuffers.size(); i++)
		{
			std::array<vk::ImageView, 1> attachments{ renderData.swapchainImageViews[i] };
			vk::FramebufferCreateInfo ci{ {}, renderData.outputPass,
				attachments.size(), attachments.data(),
				renderData.extent.width, renderData.extent.height, 1
			};
			renderData.swapchainFramebuffers[i] = renderData.device.createFramebuffer(ci);
		}
	}

	void RenderManager::createOffscreenRenderPass()
	{
		//Deferred render pass
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating deferred render pass with 3 output attachments");
		std::array<vk::AttachmentDescription, 3> deferred_descriptions = {
			vk::AttachmentDescription { {}, renderData.format.format, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			},
			vk::AttachmentDescription { {}, vk::Format::eD32Sfloat, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			},
			vk::AttachmentDescription { {}, renderData.format.format, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
			}
		};

		std::array<vk::AttachmentReference, 2> deferred_color_refs = {
			vk::AttachmentReference { 0, vk::ImageLayout::eColorAttachmentOptimal },
			vk::AttachmentReference { 2, vk::ImageLayout::eColorAttachmentOptimal}
		};
		vk::AttachmentReference deferred_depth_ref = vk::AttachmentReference{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

		vk::SubpassDescription deferred_subpass{ {}, vk::PipelineBindPoint::eGraphics,
			0, nullptr,
			(uint32_t)deferred_color_refs.size(), deferred_color_refs.data(),
			nullptr,
			&deferred_depth_ref,
			0, nullptr
		};

		const vk::RenderPassCreateInfo deferred_rp_ci{ {}, (uint32_t)deferred_descriptions.size(), deferred_descriptions.data(), 1, &deferred_subpass, 0, nullptr };
		renderData.offscreenPass = renderData.device.createRenderPass(deferred_rp_ci);
	}

	void RenderManager::createOffscreenFramebuffer()
	{
		//Deferred frame buffer
		for (auto& frame : renderData.frame)
		{
			frame.offscreenColor = createAttachment(renderData.format.format, vk::ImageAspectFlagBits::eColor, vk::ImageUsageFlagBits::eColorAttachment);
			frame.offscreenDepth = createAttachment(vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth, vk::ImageUsageFlagBits::eDepthStencilAttachment);
			frame.offscreenNormal = createAttachment(renderData.format.format, vk::ImageAspectFlagBits::eColor, vk::ImageUsageFlagBits::eColorAttachment);

			std::array<vk::ImageView, 3> offscreenViews{ frame.offscreenColor.view, frame.offscreenDepth.view, frame.offscreenNormal.view };

			Misc::Console::write("[RENDERER] [INIT] [VULKAN] Creating deferred framebuffer");
			const vk::FramebufferCreateInfo deferred_fb_ci{ {}, renderData.offscreenPass, offscreenViews, renderData.extent.width, renderData.extent.height, 1 };
			frame.offscreenFramebuffer = renderData.device.createFramebuffer(deferred_fb_ci);
		}
	}

	void RenderManager::prepareMaterials()
	{
		//Setup materials and record command buffers
		for (auto* mat : Collector<Material>::all())
		{
			mat->createDescriptorSets();
			mat->createPipeline();
		}
	}

	void RenderManager::recordOnscreenCommandBuffers() const
	{
		//Output command buffers
		Misc::Console::write("[RENDERER] [INIT] [VULKAN] Recording output command buffers");

		const vk::CommandBufferAllocateInfo cmd_info{ renderData.graphics.pool, vk::CommandBufferLevel::ePrimary, (uint32_t)renderData.swapchainFramebuffers.size() };
		renderData.onscreenCmds = renderData.device.allocateCommandBuffers(cmd_info);

		for (uint8_t i = 0; i < renderData.swapchainFramebuffers.size(); i++)
		{
			auto cmd = renderData.onscreenCmds[i];

			vk::CommandBufferBeginInfo buffer_begin{ {}, nullptr };
			cmd.begin(buffer_begin);
			{
				vk::ClearValue clear = vk::ClearValue{};
				clear.color = vk::ClearColorValue{ std::array<float, 4> { 0, 0, 1, 1 } };

				vk::RenderPassBeginInfo pass_begin{
					renderData.outputPass,
					renderData.swapchainFramebuffers[i],
					vk::Rect2D { vk::Offset2D { 0, 0 }, renderData.extent },
					1,
					&clear
				};

				cmd.beginRenderPass(pass_begin, vk::SubpassContents::eInline);
				{
					cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, screenMat->pipeline());
					cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, screenMat->layout(), 0, screenMat->set(currentFrame), {});
					cmd.draw(3, 1, 0, 0);
				}
				cmd.endRenderPass();

			}
			cmd.end();
		}
	}

	FrameBufferAttachment RenderManager::createAttachment(vk::Format format, vk::ImageAspectFlags aspect, vk::ImageUsageFlags usage)
	{
		FrameBufferAttachment result;

		const vk::ImageCreateInfo image_ci{ {}, vk::ImageType::e2D, format,
			vk::Extent3D{ renderData.extent, 1 }, 1, 1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | usage, vk::SharingMode::eExclusive,
			0, nullptr , vk::ImageLayout::eUndefined
		};
		result.image = renderData.device.createImage(image_ci);

		const auto requirements = renderData.device.getImageMemoryRequirements(result.image);
		const vk::MemoryAllocateInfo memory_info{ requirements.size, DeviceSelector::getMemoryType(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal) };
		result.memory = renderData.device.allocateMemory(memory_info);
		renderData.device.bindImageMemory(result.image, result.memory, 0);

		const vk::ImageViewCreateInfo view_ci{ {}, result.image, vk::ImageViewType::e2D, format, vk::ComponentMapping{}, vk::ImageSubresourceRange { aspect, 0, 1, 0, 1 } };
		result.view = renderData.device.createImageView(view_ci);

		const auto sampler_ci = vk::SamplerCreateInfo({},
			vk::Filter::eLinear, vk::Filter::eLinear,
			vk::SamplerMipmapMode::eLinear,
			vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
			0, VK_TRUE, 16, VK_FALSE,
			vk::CompareOp::eAlways,
			0, 0,
			vk::BorderColor::eIntOpaqueBlack,
			VK_FALSE
		);
		result.sampler = renderData.device.createSampler(sampler_ci);
		return result;
	}
}