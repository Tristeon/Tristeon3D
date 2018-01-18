#include "SkyboxVulkan.h"

#include <gli/texture_cube.hpp>
#include <gli/core/load.inl>

#include "Core/BindingData.h"
#include "HelperClasses/VulkanBuffer.h"
#include "HelperClasses/CommandBuffer.h"
#include "HelperClasses/Pipeline.h"
#include "Data/Mesh.h"
#include <gli/core/convert_func.hpp>
#include <gli/core/flip.hpp>
#include "HelperClasses/VulkanImage.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				Skybox::~Skybox()
				{
					vk::Device device = bindingData->device;

					device.destroyImage(image.img);
					device.freeMemory(image.mem);
					device.destroyImageView(image.view);
					device.destroySampler(image.sampler);
					device.freeDescriptorSets(bindingData->descriptorPool, image.set);

					device.destroyBuffer(uniformBuffer.buf);
					device.freeMemory(uniformBuffer.mem);

					device.destroyBuffer(vertexBuffer.buf);
					device.freeMemory(vertexBuffer.mem);
					
					device.destroyBuffer(indexBuffer.buf);
					device.freeMemory(indexBuffer.mem);

					device.freeCommandBuffers(bindingData->commandPool, secondary);

					delete pipeline;
				}

				void Skybox::init()
				{
					setupCubemap();
					if (!cubemapLoaded) return;
					setupPipeline();
					createUniformBuffer();
					createVertexBuffer();
					createIndexBuffer();
					createDescriptorSet();
					createCommandBuffers();
				}

				void Skybox::draw(glm::mat4 view, glm::mat4 proj)
				{
					static const size_t ubo_size = sizeof(UniformBufferObject);

					if (data == nullptr)
						return;

					ubo.model = glm::mat4(1.0);
					ubo.view = view;
					ubo.proj = proj;
					ubo.proj[1][1] *= -1;

					void* d;
					bindingData->device.mapMemory(uniformBuffer.mem, 0, ubo_size, {}, &d);
					memcpy(d, &ubo, ubo_size);
					bindingData->device.unmapMemory(uniformBuffer.mem);
					
					//Start secondary cmd buffer
					const vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &data->inheritance);
					secondary.begin(beginInfo);

					//Viewport/scissor
					secondary.setViewport(0, 1, &data->viewport);
					secondary.setScissor(0, 1, &data->scissor);
					//Pipeline
					secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->getPipeline());

					//Descriptor sets
					secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 0, 1, &image.set, 0, nullptr);

					//Vertex / index buffer
					vk::Buffer vertexBuffers[] = { vertexBuffer.buf };
					vk::DeviceSize offsets[1] = { 0 };
					secondary.bindVertexBuffers(0, 1, vertexBuffers, offsets);
					secondary.bindIndexBuffer(indexBuffer.buf, 0, vk::IndexType::eUint16);

					//Draw
					secondary.drawIndexed(mesh.indices.size(), 1, 0, 0, 0);

					//Stop secondary cmd buffer
					secondary.end();
					data->lastUsedSecondaryBuffer = secondary;
				}

				void Skybox::rebuild(vk::Extent2D extent, vk::RenderPass renderPass)
				{
					this->renderPass = renderPass;
					pipeline->rebuild(extent, renderPass);
				}

				void Skybox::setupCubemap()
				{
					//Vulkan
					vk::Device device = bindingData->device;

					//Image data
					namespace fs = std::experimental::filesystem;

					//Data::Image img = Data::ImageBatch::getImage(texturePath);
					//gli::texture t = gli::texture(gli::TARGET_CUBE, gli::FORMAT_RGBA8_UNORM_PACK8, gli::texture::extent_type(img.getWidth(), img.getHeight(), 1), 1, 6, 1);
					//memcpy(t.data(), img.getPixels(), img.getWidth() * img.getHeight() * 4);
					gli::texture t = gli::load(texturePath);
					if (t.empty())
					{
						cubemapLoaded = false;
						return;
					}

					gli::texture_cube tex(t);
					if (tex.empty())
					{
						cubemapLoaded = false;
						return;
					}
					cubemapLoaded = true;

					const vk::DeviceSize size = tex.size();
					const size_t width = tex.extent().x;
					const size_t height = tex.extent().y;
					const size_t mipLevels = tex.levels();

					//Staging buffer
					vk::Buffer sbuf;
					vk::DeviceMemory smem;
					VulkanBuffer::createBuffer(bindingData,
						size,
						vk::BufferUsageFlagBits::eTransferSrc,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						sbuf, smem);

					//Copy image data to staging buffer
					void* data;
					device.mapMemory(smem, 0, size, {}, &data);
					memcpy(data, tex.data(), size);

					//Create image
					VulkanImage::createImage(
						bindingData,
						width, height, vk::Format::eR8G8B8A8Unorm,
						vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						image.img, image.mem,
						6, vk::ImageCreateFlagBits::eCubeCompatible
					);

					//Get copy parts of the data
					std::vector<vk::BufferImageCopy> bufCopyRegions;
					size_t offset = 0;
					for (size_t face = 0; face < 6; face++)
					{
						for (size_t level = 0; level < mipLevels; level++)
						{
							bufCopyRegions.push_back(vk::BufferImageCopy(
								offset, 0, 0,
								vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, face, 1),
								vk::Offset3D(0, 0, 0),
								vk::Extent3D(tex[face][level].extent().x, tex[face][level].extent().y, 1)
							));
							offset += tex[face][level].size();
						}
					}

					//Copy buffer to image
					vk::CommandBuffer onetime = CommandBuffer::begin(bindingData->commandPool, bindingData->device);
					vk::ImageSubresourceRange const subresource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 6);

					VulkanImage::transitionImageLayout(bindingData, image.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, subresource_range);
					onetime.copyBufferToImage(sbuf, image.img, vk::ImageLayout::eTransferDstOptimal, bufCopyRegions.size(), bufCopyRegions.data());
					CommandBuffer::end(onetime, bindingData->graphicsQueue, bindingData->device, bindingData->commandPool);
					VulkanImage::transitionImageLayout(bindingData, image.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, subresource_range);

					//Create sampler
					vk::SamplerCreateInfo samp = vk::SamplerCreateInfo({},
						vk::Filter::eLinear, vk::Filter::eLinear,
						vk::SamplerMipmapMode::eLinear,
						vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge,
						0, VK_TRUE, 16.0, VK_FALSE,
						vk::CompareOp::eNever,
						0, mipLevels,
						vk::BorderColor::eIntOpaqueWhite,
						VK_FALSE);
					device.createSampler(&samp, nullptr, &image.sampler);

					//View
					vk::ImageViewCreateInfo const view = vk::ImageViewCreateInfo({},
						image.img, vk::ImageViewType::eCube,
						vk::Format::eR8G8B8A8Unorm,
						{ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA },
						vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 6)
					);
					image.view = device.createImageView(view);

					//Clear staging buffer
					device.destroyBuffer(sbuf);
					device.freeMemory(smem);
				}

				void Skybox::setupPipeline()
				{
					vk::DescriptorSetLayoutBinding const ubo = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
					vk::DescriptorSetLayoutBinding const cubesampler = vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
					std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { ubo, cubesampler };

					vk::DescriptorSetLayoutCreateInfo const ci = vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data());
					vk::DescriptorSetLayout const descriptorSetLayout = bindingData->device.createDescriptorSetLayout(ci);

					pipeline = new Pipeline(
						bindingData, 
						ShaderFile("Skybox", "Files/Shaders/", "SkyboxV", "SkyboxF"), 
						bindingData->swapchain->extent2D, 
						renderPass, 
						descriptorSetLayout,
						vk::PrimitiveTopology::eTriangleList,
						vk::CompareOp::eLessOrEqual);
				}

				void Skybox::createUniformBuffer()
				{
					VulkanBuffer::createBuffer(bindingData, sizeof(UniformBufferObject),
						vk::BufferUsageFlagBits::eUniformBuffer,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						uniformBuffer.buf, uniformBuffer.mem);
				}

				void Skybox::createCommandBuffers()
				{
					//Allocate command buffers
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(bindingData->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					bindingData->device.allocateCommandBuffers(&alloc, &secondary);
				}

				void Skybox::createDescriptorSet()
				{
					vk::DescriptorSetLayout layout = pipeline->getUniformLayout();
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(bindingData->descriptorPool, 1, &layout);
					bindingData->device.allocateDescriptorSets(&alloc, &image.set);

					//Ubo
					vk::DescriptorBufferInfo uboInfo = vk::DescriptorBufferInfo(uniformBuffer.buf, 0, sizeof(ubo));
					vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(image.set, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uboInfo, nullptr);

					//Sampler
					vk::DescriptorImageInfo samplerInfo = vk::DescriptorImageInfo(image.sampler, image.view, vk::ImageLayout::eShaderReadOnlyOptimal);
					vk::WriteDescriptorSet const samplerWrite = vk::WriteDescriptorSet(image.set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &samplerInfo, nullptr, nullptr);

					//Update descriptor with our new write info
					std::array<vk::WriteDescriptorSet, 2> writes = { uboWrite, samplerWrite };
					bindingData->device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
				}

				void Skybox::createVertexBuffer()
				{
					vk::Device device = bindingData->device;

					//Vulkan's buffers will be able to enable high performance memory when we're using a staging buffer instead of directly copying data to the gpu
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					//Create staging buffer
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(bindingData, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);

					//Upload data to staging buffer
					void* data;
					device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, mesh.vertices.data(), size);
					device.unmapMemory(stagingMemory);

					//Create vertex buffer
					VulkanBuffer::createBuffer(bindingData, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer.buf, vertexBuffer.mem);

					//Copy from staging buffer to vertex buffer
					VulkanBuffer::copyBuffer(bindingData, staging, vertexBuffer.buf, size);

					//Cleanup staging buffer
					device.destroyBuffer(staging);
					device.freeMemory(stagingMemory);
				}

				void Skybox::createIndexBuffer()
				{
					vk::DeviceSize const size = sizeof(uint16_t) * mesh.indices.size();
					if (size == 0)
						return;

					//Create staging buffer
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(bindingData, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);

					//Upload data to staging buffer
					void* data;
					bindingData->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, mesh.indices.data(), size);
					bindingData->device.unmapMemory(stagingMemory);

					//Create index buffer
					VulkanBuffer::createBuffer(bindingData, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer.buf, indexBuffer.mem);

					//Copy data from staging buffer to index buffer
					VulkanBuffer::copyBuffer(bindingData, staging, indexBuffer.buf, size);

					//Cleanup staging buffer
					bindingData->device.destroyBuffer(staging);
					bindingData->device.freeMemory(stagingMemory);
				}
			}
		}
	}
}
