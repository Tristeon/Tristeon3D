#include "CameraRenderData.h"
#include "VulkanImage.h"
#include "VulkanFormat.h"
#include "Core/Rendering/Vulkan/RenderManagerVulkan.h"
#include "Pipeline.h"
#include "Misc/Console.h"
#include "VulkanCore.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				void CameraRenderData::Offscreen::destroy(vk::Device device) const
				{
					//Cleanup
					color.destroy(device);
					depth.destroy(device);
					device.destroyFramebuffer(buffer);
					device.destroySampler(sampler);
					device.destroySemaphore(sema);
				}

				void CameraRenderData::Offscreen::init(RenderManager* rm, vk::RenderPass offscreenPass)
				{
					//Store pass
					pass = offscreenPass;
					
					//Create resources
					createImages(rm);
					createSampler(rm);
					createFramebuffer(rm);
					sema = rm->vulkan->device.createSemaphore({});

					//Allocate command buffers
					vk::CommandBufferAllocateInfo const alloc = vk::CommandBufferAllocateInfo(rm->commandPool, vk::CommandBufferLevel::ePrimary, 1);
					cmd = rm->vulkan->device.allocateCommandBuffers(alloc)[0];
				}

				void CameraRenderData::Offscreen::createImages(RenderManager* vkRenderManager)
				{
					//Get depth format
					vk::Format const depthformat = VulkanFormat::findDepthFormat(vkRenderManager->vulkan->physicalDevice);

					//Color 
					vk::Extent2D const extent = vkRenderManager->swapchain->extent2D;
					VulkanImage::createImage((VulkanBindingData*)vkRenderManager->bindingData,
						extent.width, extent.height,
						vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						color.img, color.mem);
					color.view = VulkanImage::createImageView(vkRenderManager->vulkan->device, color.img, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);

					//Depth
					VulkanImage::createImage((VulkanBindingData*)vkRenderManager->bindingData,
						extent.width, extent.height,
						depthformat, vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eDepthStencilAttachment,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						depth.img, depth.mem);
					depth.view = VulkanImage::createImageView(vkRenderManager->vulkan->device, depth.img, depthformat,
						vk::ImageAspectFlagBits::eDepth);
				}

				void CameraRenderData::Offscreen::createSampler(RenderManager* vkRenderManager)
				{
					vk::SamplerCreateInfo const smplr = vk::SamplerCreateInfo({},
						vk::Filter::eLinear, vk::Filter::eLinear,
						vk::SamplerMipmapMode::eLinear,
						vk::SamplerAddressMode::eClampToEdge,
						vk::SamplerAddressMode::eClampToEdge,
						vk::SamplerAddressMode::eClampToEdge,
						0, VK_TRUE, 1,
						VK_FALSE, vk::CompareOp::eNever,
						0, 1,
						vk::BorderColor::eFloatOpaqueWhite);
					sampler = vkRenderManager->vulkan->device.createSampler(smplr);
				}

				void CameraRenderData::Offscreen::createFramebuffer(RenderManager* vkRenderManager)
				{
					//Attaching our two images
					vk::ImageView attachments[2] = { color.view, depth.view };

					vk::Extent2D const extent = vkRenderManager->swapchain->extent2D;
					
					vk::FramebufferCreateInfo const fbuf = vk::FramebufferCreateInfo({},
						pass,
						2, attachments,
						extent.width, extent.height,
						1);
					buffer = vkRenderManager->vulkan->device.createFramebuffer(fbuf);
				}

				void CameraRenderData::Onscreen::init(Pipeline* pipeline, VulkanBindingData* binding, Offscreen offscreen, bool isEditorCam)
				{
					if (!isEditorCam)
					{
						//Setup standard descriptor sets
						auto layouts = pipeline->getDescriptorLayouts();

						//Allocate descriptor set
						vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(binding->descriptorPool, layouts.size(), layouts.data());
						vk::Result const r = binding->device.allocateDescriptorSets(&alloc, &set);
						Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate descriptor set!");

						//Describe our input
						vk::DescriptorImageInfo image = vk::DescriptorImageInfo(offscreen.sampler, offscreen.color.view, vk::ImageLayout::eShaderReadOnlyOptimal);
						vk::WriteDescriptorSet const samplerWrite = vk::WriteDescriptorSet(set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);

						//Update descriptor set with our input info
						std::array<vk::WriteDescriptorSet, 2> write = { samplerWrite };
						binding->device.updateDescriptorSets(write.size(), write.data(), 0, nullptr);
					}
					else
					{
						//Create descriptor set layout (temporary)
						vk::DescriptorSetLayoutBinding b = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
						vk::DescriptorSetLayoutCreateInfo ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &b);
						vk::DescriptorSetLayout layout = binding->device.createDescriptorSetLayout(ci);
						
						//Allocate the descriptor set
						vk::DescriptorSetAllocateInfo alloc_info = vk::DescriptorSetAllocateInfo(
							binding->descriptorPool,
							1,
							&layout);
						binding->device.allocateDescriptorSets(&alloc_info, &set);

						//Update the Descriptor Set
						vk::DescriptorImageInfo image = vk::DescriptorImageInfo(offscreen.sampler, offscreen.color.view, vk::ImageLayout::eShaderReadOnlyOptimal);
						vk::WriteDescriptorSet samplerWrite = vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);

						binding->device.updateDescriptorSets(1, &samplerWrite, 0, nullptr);
						binding->device.destroyDescriptorSetLayout(layout);
					}

					//Commandbuffer init
					vk::CommandBufferAllocateInfo const a = vk::CommandBufferAllocateInfo(binding->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					
					//Deallocate descriptor set layout again
					secondary = binding->device.allocateCommandBuffers(a)[0];
				}

				void CameraRenderData::Onscreen::destroy(vk::Device device, vk::DescriptorPool pool) const
				{
					device.freeDescriptorSets(pool, 1, &set);
				}

				CameraRenderData::CameraRenderData(RenderManager* rm, VulkanBindingData* binding, vk::RenderPass offscreenPass, Pipeline* onscreenPipeline, bool isEditorCamera) : binding(binding)
				{
					this->isEditorCam = isEditorCamera;
					init(rm, offscreenPass, onscreenPipeline);
				}

				CameraRenderData::~CameraRenderData()
				{
					offscreen.destroy(binding->device);
					onscreen.destroy(binding->device, binding->descriptorPool);
				}

				void CameraRenderData::init(RenderManager* rm, vk::RenderPass offscreenPass, Pipeline* onscreenPipeline)
				{
					offscreen.init(rm, offscreenPass);
					onscreen.init(onscreenPipeline, binding, offscreen, isEditorCam);
				}

				void CameraRenderData::rebuild(RenderManager* rm, vk::RenderPass offscreenPass, Pipeline* onscreenPipeline)
				{
					//Cleanup
					vk::CommandBuffer cmds[] = { offscreen.cmd, onscreen.secondary };
					rm->vulkan->device.freeCommandBuffers(rm->commandPool, 2, cmds);

					offscreen.destroy(rm->vulkan->device);
					onscreen.destroy(rm->vulkan->device, rm->descriptorPool);

					//Rebuild
					init(rm, offscreenPass, onscreenPipeline);
				}
			}
		}
	}
}
