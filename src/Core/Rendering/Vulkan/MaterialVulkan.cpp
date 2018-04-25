#include "MaterialVulkan.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Misc/Console.h"

#include "HelperClasses/CommandBuffer.h"
#include "HelperClasses/VulkanImage.h"
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

					//TODO: This should be a separate function for recursiveness in structs and such
					//Other data
					for (const auto pair : shader->getProps())
					{
						ShaderProperty p = pair.second;

						if (p.valueType == DT_Unknown || p.size == 0)
							continue;

						void* mem = malloc(p.size);

						switch (p.valueType)
						{
						case DT_Color:
						{
							Misc::Color const c = colors[p.name];
							glm::vec4 col = glm::vec4(c.r, c.g, c.b, c.a);
							memcpy(mem, &col, sizeof(glm::vec4));
							break;
						}
						case DT_Float:
						{
							float f = floats[p.name];
							memcpy(mem, &f, sizeof(float));
							break;
						}
						case DT_Vector3:
						{
							Math::Vector3 const vec = vectors[p.name];
							glm::vec3 v = Vec_Convert3(vec);
							memcpy(mem, &v, sizeof(glm::vec3));
							break;
						}
						case DT_Struct:
						{
							//Cast mem (malloc(p.size)) byte array 
							uint8_t* ptr = reinterpret_cast<uint8_t*>(mem);

							//Fill allocated memory with our data
							for (auto c : p.children)
							{
								switch (c.valueType)
								{
								case DT_Float:
								{
									float f = floats[p.name + "." + c.name];
									memcpy(ptr, &f, sizeof(float));
									ptr += sizeof(float);
									break;
								}
								case DT_Color:
								{
									const auto col = colors[p.name + "." + c.name];
									glm::vec4 color = glm::vec4(col.r, col.g, col.b, col.a);
									memcpy(ptr, &color, sizeof(glm::vec4));
									ptr += sizeof(glm::vec4);
									break;
								}
								case DT_Vector3:
								{
									Math::Vector3 const vec = vectors[p.name + "." + c.name];
									glm::vec3 v = glm::vec3(vec.x, vec.y, vec.z);
									memcpy(ptr, &v, sizeof(glm::vec3));
									ptr += sizeof(glm::vec3);
									break;
								}
								}
							}
							break;
						}
						default:
						{
							free(mem);
							continue;
						}
						}

						uniformBuffers[p.name]->copyFromData(mem);
						free(mem);
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
					for (const auto p : shader->getProps())
						setDefaults(p.second.name, p.second);

					//Update
					if (updateResources)
					{
						if (pipeline->getShaderFile().getNameID() != shader->getNameID())
							pipeline = RenderManager::getPipeline(*shader.get());
						if (pipeline == nullptr)
							return;
						setupTextures();
						createDescriptorSets();
					}
				}

				void Material::setDefaults(std::string name, ShaderProperty prop)
				{
					switch (prop.valueType)
					{
					case DT_Image:
						if (texturePaths.find(name) == texturePaths.end())
							texturePaths[name] = "";
						break;
					case DT_Color:
						if (colors.find(name) == colors.end())
							colors[name] = Misc::Color();
						break;
					case DT_Float:
						if (floats.find(name) == floats.end())
							floats[name] = 0;
						break;
					case DT_Vector3:
						if (vectors.find(name) == vectors.end())
							vectors[name] = Math::Vector3();
						break;
					case DT_Struct:
						for (auto c : prop.children)
						{
							std::string const n = name + "." + c.name;
							setDefaults(n, c);
						}
						//case DT_Unknown: break;
						//default: ;
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

				void Material::cleanup()
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

					uniformBuffers.clear();
					textures.clear();

					//Free descriptor set
					pipeline->device.freeDescriptorSets(pipeline->binding->descriptorPool, set);
				}

				void Material::createTextureImage(Data::Image img, Texture& texture) const
				{
					//Get image size and data
					auto const pixels = img.getPixels();
					vk::DeviceSize const size = img.getWidth() * img.getHeight() * 4;

					//Create staging buffer to allow vulkan to optimize the texture buffer's memory
					BufferVulkan staging = BufferVulkan(pipeline->binding, size, vk::BufferUsageFlagBits::eTransferSrc, 
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					staging.copyFromData(pixels);

					//Create vulkan image
					VulkanImage::createImage(
						pipeline->binding,
						img.getWidth(), img.getHeight(),
						vk::Format::eR8G8B8A8Unorm,
						vk::ImageTiling::eOptimal,
						vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
						vk::MemoryPropertyFlagBits::eDeviceLocal,
						texture.img, texture.mem);

					//Transition to transfer destination, transfer data, transition to shader read only
					VulkanImage::transitionImageLayout(pipeline->binding, texture.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
					VulkanImage::copyBufferToImage(pipeline->binding, staging.getBuffer(), texture.img, img.getWidth(), img.getHeight());
					VulkanImage::transitionImageLayout(pipeline->binding, texture.img, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

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

					std::map<std::string, vk::DescriptorImageInfo> descImgInfos;
					std::map<std::string, vk::DescriptorBufferInfo> descBufInfos;

					std::vector<vk::WriteDescriptorSet> writes;

					//Create a descriptor write instruction for each shader property
					int i = 0;
					for (const auto pair : shader->getProps())
					{
						ShaderProperty p = pair.second;
						switch (p.valueType)
						{
							case DT_Image:
							{
								descImgInfos[p.name] = vk::DescriptorImageInfo(textures[p.name].sampler, textures[p.name].view, vk::ImageLayout::eShaderReadOnlyOptimal);
								writes.push_back(vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eCombinedImageSampler, &descImgInfos[p.name], nullptr, nullptr));
								break;
							}
							case DT_Color:
							case DT_Float:
							case DT_Vector3:
							case DT_Struct:
							{
								BufferVulkan* const buf = createUniformBuffer(p.name, p.size);
								descBufInfos[p.name] = vk::DescriptorBufferInfo(buf->getBuffer(), 0, p.size);
								vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, i, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &descBufInfos[p.name], nullptr);
								writes.push_back(uboWrite);
								break;
							}
						}
						i++;
					}

					//Update descriptor with our new write info
					binding->device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
				}

				BufferVulkan* Material::createUniformBuffer(std::string name, vk::DeviceSize size)
				{
					//Create a uniform buffer of the size of UniformBufferObject
					if (uniformBuffers.find(name) != uniformBuffers.end())
						return uniformBuffers[name].get();

					VulkanBindingData* bind = pipeline->binding;
					uniformBuffers[name] = std::make_unique<BufferVulkan>(bind->device, bind->physicalDevice, bind->swapchain->getSurface(), size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					return uniformBuffers[name].get();
				}
			}
		}
	}
}