#include "MaterialVulkan.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Misc/Console.h"

#include "HelperClasses/CommandBuffer.h"
#include "HelperClasses/VulkanImage.h"
#include "HelperClasses/VulkanBuffer.h"
#include "HelperClasses/Pipeline.h"
#include "Core/BindingData.h"
#include "Misc/Hardware/Keyboard.h"
#include "RenderManagerVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				REGISTER_TYPE_CPP(Material)

				Material::~Material()
				{
					cleanup();
				}

				void Material::render(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
				{
					//Check for uniform memory
					if ((VkDeviceMemory)uniformBufferMem == VK_NULL_HANDLE)
					{
						Misc::Console::warning("Vulkan::Material::uniformBufferMem has not been set! Object's transform might be off!");
						return;
					}

					//Creat data
					UniformBufferObject ubo;
					ubo.model = model;
					ubo.view = view;
					ubo.proj = proj;
					ubo.proj[1][1] *= -1; //Vulkan with glm fix

					//Send data
					void* data;
					pipeline->device.mapMemory(uniformBufferMem, 0, sizeof(ubo), {}, &data);
					memcpy(data, &ubo, sizeof ubo);
					pipeline->device.unmapMemory(uniformBufferMem);

					//Reset so we don't acidentally use the buffer from last object
					uniformBufferMem = nullptr;
				}

				void Material::setupTextures()
				{
					createTextureImage();
					createTextureImageView();
					createTextureSampler();

					prepared = true;
				}

				void Material::setActiveUniformBufferMemory(vk::DeviceMemory uniformBuffer)
				{
					uniformBufferMem = uniformBuffer;
				}

				void Material::rebuildShader()
				{
					Vulkan::RenderManager::getPipeline(*shader.get());

				}

				void Material::cleanup() const
				{
					pipeline->device.destroySampler(diffuseSampler);
					pipeline->device.destroyImageView(diffuseView);
					pipeline->device.destroyImage(diffuseImg);
					pipeline->device.freeMemory(diffuseMemory);
				}

				void Material::createTextureImage()
				{
					//Get image size and data
					auto pixels = _diffuse.getPixels();
					vk::DeviceSize size = _diffuse.getWidth() * _diffuse.getHeight() * 4;

					//Fill with white pixels if we can't get anything else
					if (size == 0)
					{
						diffuse.freePixels(pixels);

						_diffuse = Data::Image("Files/Textures/white.jpg");
						pixels = _diffuse.getPixels();
						size = _diffuse.getWidth() * _diffuse.getHeight() * 4;
					}

					//Create staging buffer to allow vulkan to optimize the texture buffer's memory
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(pipeline->binding, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);

					//Map memory to staging buffer
					void* data;
					pipeline->binding->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, pixels, size);
					pipeline->binding->device.unmapMemory(stagingMemory);

					//Free cpu pixels
					diffuse.freePixels(pixels);

					//Create vulkan image
					VulkanImage::createImage(
						pipeline->binding,
						diffuse.getWidth(), diffuse.getHeight(),
						vk::Format::eR8G8B8A8Unorm,
						vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						diffuseImg, diffuseMemory);

					//Change texture format to transfer destination
					VulkanImage::transitionImageLayout(pipeline->binding, diffuseImg, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
					//Send data from our staging buffer to our image
					VulkanImage::copyBufferToImage(pipeline->binding, staging, diffuseImg, diffuse.getWidth(), diffuse.getHeight());
					//Change texture format to shader read only
					VulkanImage::transitionImageLayout(pipeline->binding, diffuseImg, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

					//Cleanup staging buffer
					pipeline->device.destroyBuffer(staging);
					pipeline->device.freeMemory(stagingMemory);
				}

				void Material::createTextureImageView()
				{
					//Create image view for diffuse texture
					diffuseView = VulkanImage::createImageView(pipeline->device, diffuseImg, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
				}

				void Material::createTextureSampler()
				{
					vk::SamplerCreateInfo ci = vk::SamplerCreateInfo({},
						vk::Filter::eLinear, vk::Filter::eLinear,
						vk::SamplerMipmapMode::eLinear,
						vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
						0, VK_TRUE, 16, VK_FALSE,
						vk::CompareOp::eAlways,
						0, 0,
						vk::BorderColor::eIntOpaqueBlack,
						VK_FALSE);

					vk::Result const r = pipeline->device.createSampler(&ci, nullptr, &diffuseSampler);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create image sampler: " + to_string(r));
				}

				void Material::property__set_diffuse(const property__tmp_type_diffuse& value)
				{
					_diffuse = value;

					if (prepared)
					{
						cleanup();
						pipeline->device.freeDescriptorSets(pipeline->binding->descriptorPool, 1, &set);
						setupTextures();
						createDescriptorSets();
					}
				}

				void Material::createDescriptorSets()
				{
					VulkanBindingData* binding = pipeline->binding; //Easy reference for now

					//Allocate
					vk::DescriptorSetLayout layout = pipeline->getSamplerLayout();
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout);
					vk::Result const r = binding->device.allocateDescriptorSets(&alloc, &set);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate descriptor set!");

					//Write info
					vk::DescriptorImageInfo image = vk::DescriptorImageInfo(diffuseSampler, diffuseView, vk::ImageLayout::eShaderReadOnlyOptimal);
					vk::WriteDescriptorSet const samplerWrite = vk::WriteDescriptorSet(set, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image, nullptr, nullptr);

					//Update descriptor with our new write info
					std::array<vk::WriteDescriptorSet, 1> write = { samplerWrite };
					binding->device.updateDescriptorSets(write.size(), write.data(), 0, nullptr);
				}
			}
		}
	}
}
