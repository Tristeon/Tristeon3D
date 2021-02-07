#include "SkyboxVulkan.h"

#include <gli/texture_cube.hpp>
#include <gli/core/load.inl>

#include "HelperClasses/CommandBuffer.h"
#include "HelperClasses/Pipeline.h"
#include "Data/Mesh.h"
#include <gli/core/convert_func.hpp>
#include <gli/core/flip.hpp>

#include "Core/BindingData.h"
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
					vk::Device device = binding_data.device;

					device.destroyImage(image.img);
					device.freeMemory(image.mem);
					device.destroyImageView(image.view);
					device.destroySampler(image.sampler);
					device.freeDescriptorSets(binding_data.descriptorPool, image.set);

					device.freeCommandBuffers(binding_data.commandPool, secondary);

					device.freeDescriptorSets(binding_data.descriptorPool, lightingSet);
					delete pipeline;
				}

				void Skybox::init()
				{
					setupCubemap();
					if (!cubemapLoaded)
					{
						Misc::Console::warning("Failed to load Skybox map " + texturePath);
						return;
					}
					setupPipeline();
					createUniformBuffer();
					createVertexBuffer();
					createIndexBuffer();
					if (!vertexBuffer || !indexBuffer)
						Misc::Console::warning("Failed to load Skybox model!");
					createDescriptorSet();
					createCommandBuffers();
					createOffscreenDescriptorSet();
				}

				void Skybox::draw(glm::mat4 view, glm::mat4 proj)
				{
					if (data == nullptr)
						return;

					if (vertexBuffer == nullptr || indexBuffer == nullptr)
						return;

					ubo.model = glm::mat4(1.0);
					ubo.view = view;
					ubo.proj = proj;
					ubo.proj[1][1] *= -1;

					uniformBuffer->copyFromData(&ubo);
					
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
					vk::Buffer vertexBuffers[] = { vertexBuffer->getBuffer() };
					vk::DeviceSize offsets[1] = { 0 };
					secondary.bindVertexBuffers(0, 1, vertexBuffers, offsets);
					secondary.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint16);

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
					//Image data
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

					BufferVulkan staging = BufferVulkan(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					staging.copyFromData(tex.data());

					//Create image
					VulkanImage::createImage(
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
					vk::CommandBuffer onetime = CommandBuffer::begin();
					vk::ImageSubresourceRange const subresource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 6);

					VulkanImage::transitionImageLayout(image.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, subresource_range);
					onetime.copyBufferToImage(staging.getBuffer(), image.img, vk::ImageLayout::eTransferDstOptimal, bufCopyRegions.size(), bufCopyRegions.data());
					CommandBuffer::end(onetime);
					VulkanImage::transitionImageLayout(image.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, subresource_range);

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
					binding_data.device.createSampler(&samp, nullptr, &image.sampler);

					//View
					vk::ImageViewCreateInfo const view = vk::ImageViewCreateInfo({},
						image.img, vk::ImageViewType::eCube,
						vk::Format::eR8G8B8A8Unorm,
						{ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA },
						vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 6)
					);
					image.view = binding_data.device.createImageView(view);
				}

				void Skybox::setupPipeline()
				{

					vk::DescriptorSetLayoutBinding const ubo = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
					vk::DescriptorSetLayoutBinding const cubesampler = vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
					std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { ubo, cubesampler };

					vk::DescriptorSetLayoutCreateInfo const ci = vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data());
					vk::DescriptorSetLayout const descriptorSetLayout = binding_data.device.createDescriptorSetLayout(ci);

					pipeline = new Pipeline(
						ShaderFile("Skybox", "Files/Shaders/", "SkyboxV", "SkyboxF"), 
						binding_data.extent,
						renderPass, 
						descriptorSetLayout,
						vk::PrimitiveTopology::eTriangleList,
						vk::CompareOp::eLessOrEqual,
						vk::CullModeFlagBits::eFront);
				}

				void Skybox::createUniformBuffer()
				{
					uniformBuffer = std::make_unique<BufferVulkan>(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, 
						vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
				}

				void Skybox::createCommandBuffers()
				{
					//Allocate command buffers
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(binding_data.commandPool, vk::CommandBufferLevel::eSecondary, 1);
					binding_data.device.allocateCommandBuffers(&alloc, &secondary);
				}

				void Skybox::createDescriptorSet()
				{
					vk::DescriptorSetLayout layout = pipeline->getUniformLayout();
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(binding_data.descriptorPool, 1, &layout);
					binding_data.device.allocateDescriptorSets(&alloc, &image.set);

					//Ubo
					vk::DescriptorBufferInfo uboInfo = vk::DescriptorBufferInfo(uniformBuffer->getBuffer(), 0, sizeof(ubo));
					vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(image.set, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uboInfo, nullptr);

					//Sampler
					vk::DescriptorImageInfo samplerInfo = vk::DescriptorImageInfo(image.sampler, image.view, vk::ImageLayout::eShaderReadOnlyOptimal);
					vk::WriteDescriptorSet const samplerWrite = vk::WriteDescriptorSet(image.set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &samplerInfo, nullptr, nullptr);

					//Update descriptor with our new write info
					std::array<vk::WriteDescriptorSet, 2> writes = { uboWrite, samplerWrite };
					binding_data.device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
				}

				void Skybox::createOffscreenDescriptorSet()
				{
					vk::DescriptorSetLayoutBinding const s = vk::DescriptorSetLayoutBinding(
						0, vk::DescriptorType::eCombinedImageSampler,
						1, vk::ShaderStageFlagBits::eFragment,
						nullptr);
					vk::DescriptorSetLayout layout;
					vk::DescriptorSetLayoutCreateInfo cil = vk::DescriptorSetLayoutCreateInfo({}, 1, &s);
					binding_data.device.createDescriptorSetLayout(&cil, nullptr, &layout);

					vk::DescriptorSetAllocateInfo const alloc = vk::DescriptorSetAllocateInfo(binding_data.descriptorPool, 1, &layout);
					lightingSet = binding_data.device.allocateDescriptorSets(alloc)[0];

					vk::DescriptorImageInfo img = vk::DescriptorImageInfo(image.sampler, image.view, vk::ImageLayout::eShaderReadOnlyOptimal);
					vk::WriteDescriptorSet write = vk::WriteDescriptorSet(lightingSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &img, nullptr, nullptr);

					binding_data.device.updateDescriptorSets(1, &write, 0, nullptr);

					binding_data.device.destroyDescriptorSetLayout(layout);
				}

				void Skybox::createVertexBuffer()
				{
					//Vulkan's buffers will be able to enable high performance memory when we're using a staging buffer instead of directly copying data to the gpu
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					vertexBuffer = BufferVulkan::createOptimized(size, mesh.vertices.data(), vk::BufferUsageFlagBits::eVertexBuffer);
				}

				void Skybox::createIndexBuffer()
				{
					vk::DeviceSize const size = sizeof(uint16_t) * mesh.indices.size();
					if (size == 0)
						return;

					indexBuffer = BufferVulkan::createOptimized(size, mesh.indices.data(), vk::BufferUsageFlagBits::eIndexBuffer);
				}
			}
		}
	}
}
