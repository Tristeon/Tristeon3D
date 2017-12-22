#include "CameraRenderData.h"
#include "VulkanImage.h"
#include "VulkanFormat.h"
#include "Core/Rendering/Vulkan/RenderManagerVulkan.h"
#include "Pipeline.h"
#include "Misc/Console.h"
#include "VulkanCore.h"
#include "VulkanBuffer.h"
#include "Core/Rendering/Vulkan/MaterialVulkan.h"
#include <gli/gli.hpp>

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

				void CameraRenderData::Offscreen::init(RenderManager* rm, vk::RenderPass offscreenPass)// , Skybox* skybox)
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

					/*
					if (skybox != nullptr)
					{
						size_t size = skybox->getSize();

						vk::Buffer stagingBuf;
						vk::DeviceMemory stagingMem;

						vk::BufferCreateInfo stagBufCI = vk::BufferCreateInfo({}, 
							size, 
							vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
							)


						vk::MemoryAllocateInfo alloc = vk::MemoryAllocateInfo()
					}
					*/
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

				void CameraRenderData::Onscreen::init(VulkanBindingData* binding, Offscreen offscreen, bool isEditorCam, Pipeline* onscreenPipeline)
				{
					//The editor camera has its own descriptor set due to ImGUI binding settings
					if (isEditorCam)
					{
						//Descriptor set layout containing one image sampler
						//ImGUI's shaders take 1 image sampler in the fragment shader. nothing else
						vk::DescriptorSetLayoutBinding b = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
						vk::DescriptorSetLayoutCreateInfo const ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &b);
						vk::DescriptorSetLayout layout = binding->device.createDescriptorSetLayout(ci);

						//Allocate the descriptor set with our layout
						vk::DescriptorSetAllocateInfo alloc_info = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout);
						binding->device.allocateDescriptorSets(&alloc_info, &sets[0]);

						//Update the descriptor set with our offscreen rendered image
						vk::DescriptorImageInfo image = vk::DescriptorImageInfo(offscreen.sampler, offscreen.color.view, vk::ImageLayout::eShaderReadOnlyOptimal);
						vk::WriteDescriptorSet samplerWrite = vk::WriteDescriptorSet(sets[0], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);
						binding->device.updateDescriptorSets(1, &samplerWrite, 0, nullptr);
						
						//Clean up the descriptor set layout as we don't need it anymore
						binding->device.destroyDescriptorSetLayout(layout);
					}
					else
					{
						//Cameras don't actually use uniform buffers right now, but due to our
						//pipeline setup we unfortunately have to include a buffer in some form or way,
						//so we'll just create a very small uniform buffer

						//Descriptor layout for the uniform buffer
						vk::DescriptorSetLayout layout1 = onscreenPipeline->getUniformLayout();
						//Uniform buffer descriptor set creation
						vk::DescriptorSetAllocateInfo alloc1 = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout1);
						binding->device.allocateDescriptorSets(&alloc1, &sets[0]);

						//Create a temporary buffer so we can fit the pipeline descriptor set layout
						VulkanBuffer::createBuffer(binding,
							1, vk::BufferUsageFlagBits::eUniformBuffer,
							vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
							tempBuf, tempBufMem);
						//Update the descriptor set with our "buffer"
						vk::DescriptorBufferInfo bufi = vk::DescriptorBufferInfo(tempBuf, 0, 1);
						vk::WriteDescriptorSet buf = vk::WriteDescriptorSet(sets[0], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufi, nullptr);
						binding->device.updateDescriptorSets(1, &buf, 0, nullptr);
						
						//The sampler is used in the fragment shader to show the texture on screen
						//We're binding the color texture that our offscreen camera is rendering to 
						//to this shader pipeline
						//Get the layout and create the descriptor set
						vk::DescriptorSetLayout layout2 = onscreenPipeline->getSamplerLayout();
						vk::DescriptorSetAllocateInfo alloc_info = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout2);
						binding->device.allocateDescriptorSets(&alloc_info, &sets[1]);

						//Update the descriptor set with our sampler and our offscreen view
						vk::DescriptorImageInfo image = vk::DescriptorImageInfo(offscreen.sampler, offscreen.color.view, vk::ImageLayout::eShaderReadOnlyOptimal);
						vk::WriteDescriptorSet samplerWrite = vk::WriteDescriptorSet(sets[1], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);
						binding->device.updateDescriptorSets(1, &samplerWrite, 0, nullptr);
					}

					//Create the command buffer used to render this camera's texture to the screen
					vk::CommandBufferAllocateInfo const a = vk::CommandBufferAllocateInfo(binding->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					secondary = binding->device.allocateCommandBuffers(a)[0];
				}

				void CameraRenderData::Onscreen::destroy(vk::Device device, vk::DescriptorPool pool) const
				{
					//Clean up the resources that we've created
					device.destroyBuffer(tempBuf);
					device.freeMemory(tempBufMem);
					device.freeDescriptorSets(pool, 2, sets.data());
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
					onscreen.init(binding, offscreen, isEditorCam, onscreenPipeline);
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
