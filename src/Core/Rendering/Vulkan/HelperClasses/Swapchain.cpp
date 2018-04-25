#include "Swapchain.h"
#include "QueueFamilyIndices.h"
#include "Misc/Console.h"
#include "VulkanImage.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				SwapChainSupportDetails::SwapChainSupportDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface)
				{
					//Get support details
					capabilities = device.getSurfaceCapabilitiesKHR(surface);
					formats = device.getSurfaceFormatsKHR(surface);
					presentModes = device.getSurfacePresentModesKHR(surface);
				}

				vk::SurfaceFormatKHR SwapChainSupportDetails::chooseFormat()
				{
					//Undefined? take our ideal choice
					if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
						return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

					for (const vk::SurfaceFormatKHR & format : formats)
					{
						//Try to find rgba8 with nonlinear colorspace
						if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
							return format;
					}
						
					//Else just return whatever
					return formats[0];
				}

				vk::PresentModeKHR SwapChainSupportDetails::chooseMode()
				{
					for (const vk::PresentModeKHR& mode : presentModes)
					{
						//Emailbox is ideal because it allows for triple buffering
						if (mode == vk::PresentModeKHR::eMailbox)
							return mode;
					}

					//Fifo always works
					return vk::PresentModeKHR::eFifo;
				}

				vk::Extent2D SwapChainSupportDetails::chooseExtent(int windowWidth, int windowHeight) const
				{
					VkExtent2D actualExtent = { windowWidth, windowHeight };
					//Not smaller than minimum, not bigger than maximum
					actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
					actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
					return actualExtent;
				}

				uint32_t SwapChainSupportDetails::getImageCount() const
				{
					//Try to push it one higher than minimum, to potentially allow for triple buffering
					uint32_t imageCount = capabilities.minImageCount + 1;
					if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
						imageCount = capabilities.maxImageCount;

					return imageCount;
				}

				Swapchain::Swapchain(vk::Device device, vk::PhysicalDevice gpu, vk::SurfaceKHR surface, int width, int height)
				{
					//Init
					this->device = device;
					this->physicalDevice = gpu;
					this->surface = surface;
					create(width, height);
				}

				Swapchain::~Swapchain()
				{
					//Clear all resources created by swapchain
					cleanup();
				}

				void Swapchain::createSwapchain(int width, int height)
				{
					//Info
					const QueueFamilyIndices indices = QueueFamilyIndices::get(physicalDevice, surface);
					SwapChainSupportDetails support = SwapChainSupportDetails(physicalDevice, surface);

					//Choose format, presentMode and extent
					const vk::SurfaceFormatKHR format = support.chooseFormat();
					this->_format = format.format;
					const vk::PresentModeKHR mode = support.chooseMode();
					extent = support.chooseExtent(width, height);
					const uint32_t imageCount = support.getImageCount();

					vk::SharingMode sharingMode;
					int queueFamilyIndexCount;

					uint32_t* queueFamilyIndices;
					
					//Concurrent due to graphics and present family being different
					if (indices.graphicsFamily != indices.presentFamily)
					{
						sharingMode = vk::SharingMode::eConcurrent;
						queueFamilyIndexCount = 2;
						queueFamilyIndices = new uint32_t[2]{ (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };
					}
					//Exclusive, higher in performance :)
					else
					{
						sharingMode = vk::SharingMode::eExclusive;
						queueFamilyIndexCount = 0;
						queueFamilyIndices = nullptr;
					}

					//Create swapchain
					vk::SwapchainCreateInfoKHR sci = vk::SwapchainCreateInfoKHR(
						vk::SwapchainCreateFlagsKHR(), surface,
						imageCount,
						format.format, format.colorSpace,
						extent, 1,
						vk::ImageUsageFlagBits::eColorAttachment, sharingMode,
						queueFamilyIndexCount, queueFamilyIndices,
						support.capabilities.currentTransform,
						vk::CompositeAlphaFlagBitsKHR::eOpaque, mode,
						true, nullptr);

					const vk::Result r = device.createSwapchainKHR(&sci, nullptr, &swapChain);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create swapchain!");
				}

				void Swapchain::createRenderPass()
				{
					//Color attachment description
					vk::AttachmentDescription const colorAttachment = vk::AttachmentDescription({},
						format, vk::SampleCountFlagBits::e1,
						vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
						vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
						vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

					//Binding position/reference
					vk::AttachmentReference colorRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
					std::array<vk::AttachmentDescription, 1> attachments = { colorAttachment };

					//Subpass
					vk::SubpassDescription subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics,
						0, nullptr,
						1, &colorRef,
						nullptr, nullptr,
						0, nullptr);

					vk::SubpassDependency dependency = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
						vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
						{}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, {});

					//Create renderpass
					vk::RenderPassCreateInfo ci = vk::RenderPassCreateInfo({}, attachments.size(), attachments.data(), 1, &subpass, 1, &dependency);
					const vk::Result r = device.createRenderPass(&ci, nullptr, &_renderPass);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create render pass!");
				}

				void Swapchain::createFramebuffers()
				{
					//A framebuffer for every swapchain image view
					framebuffers.resize(imageViews.size());

					for(size_t i = 0; i < framebuffers.size(); i++)
					{
						//Framebuffer attachments
						std::array<vk::ImageView, 1> attachments = { imageViews[i] };

						//Create framebuffer
						vk::FramebufferCreateInfo ci = vk::FramebufferCreateInfo(
							{}, renderpass, 
							attachments.size(), attachments.data(), 
							extent.width, extent.height, 
							1);
						const vk::Result r = device.createFramebuffer(&ci, nullptr, &framebuffers[i]);
						Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create framebuffer");
					}
				}

				void Swapchain::cleanup()
				{
					//Destroy all resources allocated by swapchain
					for (int i = 0; i < framebuffers.size(); i++)
						device.destroyFramebuffer(framebuffers[i]);

					device.destroyRenderPass(_renderPass);

					for (size_t i = 0; i < imageViews.size(); i++)
						device.destroyImageView(imageViews[i]);

					device.destroySwapchainKHR(swapChain);
				}

				void Swapchain::createImageViews()
				{
					//Get swapchain images
					images = device.getSwapchainImagesKHR(swapChain);
					imageViews.resize(images.size());

					//Create an image view for each swapchain image. These image views are later on used as an attachment to framebuffers
					for (int i = 0; i < imageViews.size(); i++)
						imageViews[i] = VulkanImage::createImageView(device, images[i], format, vk::ImageAspectFlagBits::eColor);
				}

				void Swapchain::rebuild(int newWidth, int newHeight)
				{
					device.waitIdle();

					cleanup();
					create(newWidth, newHeight);
				}

				void Swapchain::create(int width, int height)
				{
					/**
					 * - Create swapchain, load swapchain images
					 * - Create swapchain image views
					 * - Create renderpass
					 * - Pass swapchain image view as attachment to swapchain framebuffer
					 */
					createSwapchain(width, height);
					createImageViews();
					createRenderPass();
				}
			}
		}
	}
}
