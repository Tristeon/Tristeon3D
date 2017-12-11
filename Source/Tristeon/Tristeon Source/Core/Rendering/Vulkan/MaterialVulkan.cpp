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
#include "Data/ImageBatch.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				REGISTER_TYPE_CPP(Vulkan::Material)

				Material::~Material()
				{
					cleanup();
				}

				void Material::render(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
				{
					if ((VkDeviceMemory)uniformBufferMem == VK_NULL_HANDLE)
					{
						Misc::Console::warning("Vulkan::Material::uniformBufferMem has not been set! Object's transform will be off!");
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

					//Verify data
					if (shader == nullptr)
					{
						Misc::Console::warning("Material.shader is nullptr. Object's material properties will be off!");
						return;
					}
					if (pipeline == nullptr)
					{
						Misc::Console::warning("Vulkan::Material.pipeline is nullptr. Object's material properties will be off!");
						return;
					}

					//Other data
					for (const auto p : shader->getProperties())
					{
						// ReSharper disable once CppJoinDeclarationAndAssignment - Using mem address, has to stay out of scope
						glm::vec3 v3;
						// ReSharper disable once CppJoinDeclarationAndAssignment - Using mem address, has to stay out of scope
						glm::vec4 v4;

						vk::DeviceSize size;
						void* mem;

						switch(p.valueType)
						{
							case DT_Image: 
								continue;
							case DT_Color:
							{
								size = sizeof(glm::vec4);
								Misc::Color c = colors[p.name];
								// ReSharper disable once CppJoinDeclarationAndAssignment - Using mem address, has to stay out of scope
								v4 = { c.r, c.g, c.b, c.a };
								mem = &v4;
								break;
							}
							case DT_Float:
							{
								size = sizeof(float);
								mem = &floats[p.name];
								break;
							}
							case DT_Vector3:
							{
								size = sizeof(glm::vec3);
								Math::Vector3 const vec = vectors[p.name];
								// ReSharper disable once CppJoinDeclarationAndAssignment - Using mem address, has to stay out of scope
								v3 = { vec.x, vec.y, vec.z };
								mem = &v3;
								break;
							}
							default:
								continue;
						}

						void* d;
						pipeline->device.mapMemory(uniformBuffers[p.name].mem, 0, size, {}, &d);
						memcpy(d, mem, size);
						pipeline->device.unmapMemory(uniformBuffers[p.name].mem);
					}

					//Reset so we don't acidentally use the buffer from last object
					uniformBufferMem = nullptr;
				}

				void Material::setupTextures()
				{
					for (const auto t : texturePaths)
					{
						Texture vktex;
						createTextureImage(Data::Image(t.second), vktex);
						createTextureSampler(vktex);
						textures[t.first] = vktex;
					}
				}

				void Material::setActiveUniformBufferMemory(vk::DeviceMemory uniformBuffer)
				{
					uniformBufferMem = uniformBuffer;
				}

				void Material::updateProperties(bool updateResources)
				{
					cleanup();

					if (shader == nullptr)
						return;

					//Fill in empty vars
					for (const auto p : shader->getProperties())
					{
						switch(p.valueType)
						{
						case DT_Image: 
							if (texturePaths.find(p.name) == texturePaths.end())
								texturePaths[p.name] = "";
							break;
						case DT_Color: 
							if (colors.find(p.name) == colors.end())
								colors[p.name] = Misc::Color();
							break;
						case DT_Float: 
							if (floats.find(p.name) == floats.end())
								floats[p.name] = 0;
							break;
						case DT_Vector3:
							if (vectors.find(p.name) == vectors.end())
								vectors[p.name] = Math::Vector3();
							break;
						default: ;
						}
					}

					//Update
					if (updateResources)
					{
						pipeline = RenderManager::getPipeline(*shader.get());
						if (pipeline == nullptr)
							return;
						setupTextures();
						createDescriptorSets();
					}
				}

				void Material::updateShader()
				{
					Rendering::Material::updateShader();
					if (shader == nullptr)
						return;
					pipeline = RenderManager::getPipeline(*shader.get());
				}

				void Material::setTexture(std::string name, std::string path)
				{
					Rendering::Material::setTexture(name, path);

					//Update image if it already exists. Otherwise we can safely assume that either 
					//this texture doesn't exist, or our textures have not been set up yet
					if (textures.find(name) != textures.end())
					{
						Texture tex = textures[name];
						pipeline->device.destroyImage(tex.img);
						pipeline->device.freeMemory(tex.mem);
						pipeline->device.destroyImageView(tex.view);
						pipeline->device.destroySampler(tex.sampler);

						createTextureImage(Data::ImageBatch::getImage(path), tex);
						createTextureSampler(tex);

						textures[name] = tex;

						pipeline->device.freeDescriptorSets(pipeline->binding->descriptorPool, set);
						createDescriptorSets();
					}
				}

				void Material::cleanup() const
				{
					//None of this has been created if we don't have a pipeline, nor can we destroy anything without pipeline anyways
					if (pipeline == nullptr)
						return;

					//Destroy textures
					for (auto const t : textures)
					{
						pipeline->device.destroySampler(t.second.sampler);
						pipeline->device.destroyImageView(t.second.view);
						pipeline->device.destroyImage(t.second.img);
						pipeline->device.freeMemory(t.second.mem);
					}
					//Destroy uniform buffers
					for (auto const b : uniformBuffers)
					{
						pipeline->device.destroyBuffer(b.second.buf);
						pipeline->device.freeMemory(b.second.mem);
					}
					//Free descriptor set
					pipeline->device.freeDescriptorSets(pipeline->binding->descriptorPool, set);
				}

				void Material::createTextureImage(Data::Image img, Texture& texture) const
				{
					//Get image size and data
					auto pixels = img.getPixels();
					vk::DeviceSize size = img.getWidth() * img.getHeight() * 4;

					//Create staging buffer to allow vulkan to optimize the texture buffer's memory
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(pipeline->binding, size,
						vk::BufferUsageFlagBits::eTransferSrc,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						staging, stagingMemory);

					//Upload our memory to the staging buffer
					void* data;
					pipeline->binding->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, pixels, size);
					pipeline->binding->device.unmapMemory(stagingMemory);

					//Create vulkan image
					VulkanImage::createImage(
						pipeline->binding,
						img.getWidth(), img.getHeight(),
						vk::Format::eR8G8B8A8Unorm,
						vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						texture.img, texture.mem);

					//Change texture format to transfer destination
					VulkanImage::transitionImageLayout(pipeline->binding, texture.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
					//Send data from our staging buffer to our image
					VulkanImage::copyBufferToImage(pipeline->binding, staging, texture.img, img.getWidth(), img.getHeight());
					//Change texture format to shader read only
					VulkanImage::transitionImageLayout(pipeline->binding, texture.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

					//Cleanup staging buffer
					pipeline->device.destroyBuffer(staging);
					pipeline->device.freeMemory(stagingMemory);

					//Create image view for texture
					texture.view = VulkanImage::createImageView(pipeline->device, texture.img, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
				}

				void Material::createTextureSampler(Texture& tex) const
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

					vk::Result const r = pipeline->device.createSampler(&ci, nullptr, &tex.sampler);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to create image sampler: " + to_string(r));
				}

				void Material::createDescriptorSets()
				{
					//Don't bother if we don't even have a shader pipeline
					if (shader == nullptr || pipeline == nullptr)
						return;
					VulkanBindingData* binding = pipeline->binding; //Reference

					//Allocate the descriptor set we're using to pass material properties to the shader
					vk::DescriptorSetLayout layout = pipeline->getSamplerLayout();
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout);
					vk::Result const r = binding->device.allocateDescriptorSets(&alloc, &set);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate descriptor set!");

					std::vector<vk::WriteDescriptorSet> writes;

					//Create a descriptor write instruction for each shader property
					int i = 0;
					for (const auto p : shader->getProperties())
					{
						print("Binding material property " + p.name);
						switch(p.valueType)
						{
							case DT_Image:
							{
								vk::DescriptorImageInfo img = vk::DescriptorImageInfo(textures[p.name].sampler, textures[p.name].view, vk::ImageLayout::eShaderReadOnlyOptimal);
								writes.push_back(vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eCombinedImageSampler, &img, nullptr, nullptr));
								break;
							}
							case DT_Color:
							{
								UniformBuffer const buf = createUniformBuffer(p.name);
								vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(buf.buf, 0, sizeof(glm::vec4));
								vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &buffer, nullptr);
								writes.push_back(uboWrite);
								uniformBuffers[p.name] = buf;
								break;
							}
							case DT_Float: 
							{
								UniformBuffer const buf = createUniformBuffer(p.name);
								vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(buf.buf, 0, sizeof(float));
								vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &buffer, nullptr);
								writes.push_back(uboWrite);
								uniformBuffers[p.name] = buf;
								break;
							}
							case DT_Vector3: 
							{
								UniformBuffer const buf = createUniformBuffer(p.name);
								vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(buf.buf, 0, sizeof(glm::vec3));
								vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &buffer, nullptr);
								writes.push_back(uboWrite);
								break;
							}
						}
						i++;
					}

					//Update descriptor with our new write info
					binding->device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
				}

				UniformBuffer Material::createUniformBuffer(std::string name)
				{
					UniformBuffer buf;
					//Create a uniform buffer of the size of UniformBufferObject
					if ((VkBuffer)uniformBuffers[name].buf != VK_NULL_HANDLE)
					{
						pipeline->device.destroyBuffer(uniformBuffers[name].buf);
						pipeline->device.freeMemory(uniformBuffers[name].mem);
					}
					VulkanBuffer::createBuffer(pipeline->binding, sizeof(glm::vec3),
						vk::BufferUsageFlagBits::eUniformBuffer,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						buf.buf, buf.mem);
					uniformBuffers[name] = buf;
					return buf;
				}
			}
		}
	}
}
