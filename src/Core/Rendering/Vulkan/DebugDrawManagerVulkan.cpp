#include "DebugDrawManagerVulkan.h"
#include "InternalMeshRendererVulkan.h"
#include "MaterialVulkan.h"
#include "Core/BindingData.h"
#include "HelperClasses/Pipeline.h"
#include "Core/Components/Camera.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				DebugDrawManager::DebugDrawManager(vk::RenderPass offscreenPass) : offscreenPass(offscreenPass)
				{
					file = ShaderFile("Line", "Files/Shaders/", "LineV", "LineF");

					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(VulkanBindingData::getInstance()->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					VulkanBindingData::getInstance()->device.allocateCommandBuffers(&alloc, &cmd);
					createDescriptorSets();
				}

				DebugDrawManager::~DebugDrawManager()
				{
					VulkanBindingData* bindingData = VulkanBindingData::getInstance();
					bindingData->device.freeCommandBuffers(bindingData->commandPool, cmd);
					bindingData->device.freeDescriptorSets(bindingData->descriptorPool, set);
				}

				void DebugDrawManager::render()
				{
					if (data == nullptr)
						return;
					if (drawList.size() == 0)
						return;

					//Get our material, and render it with the meshrenderer's model matrix
					glm::mat4 const model = glm::mat4(1.0f);

					//Start secondary cmd buffer
					const vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(
						vk::CommandBufferUsageFlagBits::eRenderPassContinue,
						&data->inheritance);

					vk::CommandBuffer secondary = cmd;
					secondary.begin(beginInfo);

					//Viewport/scissor
					secondary.setViewport(0, 1, &data->viewport);
					secondary.setScissor(0, 1, &data->scissor);

					size_t materialIndex = 0;
					size_t meshIndex = 0;
					
					//Separate by color. Each color gets a separate material
					for (const auto& colorSet : drawList)
					{
						Misc::Color const color = colorSet.first;

						//Get material, add new one if necessary
						if (materialIndex >= materials.size())
							addMaterial();
						Material* m = materials[materialIndex].get();
						materialIndex++;
						
						//Apply material properties
						secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipeline());

						vk::DescriptorSet sets[] = { set, m->set };
						secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipelineLayout(), 0, 2, sets, 0, nullptr);

						m->setColor("Color.color", color);
						m->setActiveUniformBufferMemory(uniformBuffer->getDeviceMemory());
						m->render(model, data->view, data->projection);

						//Separate by width, each width gets a separate vertexbuffer
						for (const auto& widthSet : colorSet.second)
						{
							std::queue<DebugMesh> meshes = widthSet.second;
							float const width = widthSet.first;

							//Combine meshes with the same color and width
							while (!meshes.empty())
							{
								std::vector<Data::Vertex> vertices = meshes.front().vertices;
								meshes.pop();

								//Send vertex data to the GPU
								if (meshIndex >= vertexBuffers.size())
									vertexBuffers.push_back(nullptr);
								BufferVulkan* buffer = createVertexBuffer(vertices, meshIndex);
								if (buffer == nullptr) continue;
								meshIndex++;

								vk::DeviceSize offsets[1] = { 0 };
								vk::Buffer vertex = buffer->getBuffer();
								secondary.bindVertexBuffers(0, 1, &vertex, offsets);

								//Update line width
								secondary.setLineWidth(width);

								//Draw
								secondary.draw((uint32_t)vertices.size(), 1, 0, 0);
							}
						}
					}

					//Stop secondary cmd buffer
					secondary.end();

					data->lastUsedSecondaryBuffer = cmd;

					clear();
				}

				void DebugDrawManager::createDescriptorSets()
				{
					VulkanBindingData* bindingData = VulkanBindingData::getInstance();

					//Create a uniform buffer of the size of UniformBufferObject
					uniformBuffer = std::make_unique<BufferVulkan>(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

					//Create a temporary layout describing the uniform buffer input
					vk::DescriptorSetLayout layout;
					vk::DescriptorSetLayoutBinding const ubo = vk::DescriptorSetLayoutBinding(
						0, vk::DescriptorType::eUniformBuffer,
						1, vk::ShaderStageFlagBits::eVertex,
						nullptr);
					vk::DescriptorSetLayoutCreateInfo ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &ubo);
					bindingData->device.createDescriptorSetLayout(&ci, nullptr, &layout);

					//Allocate
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(bindingData->descriptorPool, 1, &layout);
					vk::Result const r = bindingData->device.allocateDescriptorSets(&alloc, &set);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate descriptor set!");

					//Write info
					vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(uniformBuffer->getBuffer(), 0, sizeof(UniformBufferObject));
					vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &buffer, nullptr);

					//Update descriptor with our new write info
					std::array<vk::WriteDescriptorSet, 1> write = { uboWrite };
					bindingData->device.updateDescriptorSets((uint32_t)write.size(), write.data(), 0, nullptr);

					bindingData->device.destroyDescriptorSetLayout(layout);
				}

				void DebugDrawManager::onResize(vk::RenderPass offscreenPass)
				{
					this->offscreenPass = offscreenPass;
					for (const std::unique_ptr<Pipeline>& pipeline : pipelines)
						pipeline->rebuild(VulkanBindingData::getInstance()->swapchain->extent2D, offscreenPass);
				}

				BufferVulkan* DebugDrawManager::createVertexBuffer(std::vector<Data::Vertex> vertices, int i)
				{
					vk::DeviceSize const size = sizeof(Data::Vertex) * vertices.size();
					if (size == 0)
						return nullptr;

					if (!vertexBuffers[i])
						vertexBuffers[i] = std::make_unique<BufferVulkan>(size, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					vertexBuffers[i]->copyFromData(vertices.data());

					return vertexBuffers[i].get();
				}

				void DebugDrawManager::addMaterial()
				{
					Pipeline* pipeline = new Pipeline(file, VulkanBindingData::getInstance()->swapchain->extent2D, offscreenPass, true, vk::PrimitiveTopology::eLineList);

					Material* material = new Vulkan::Material();
					material->pipeline = pipeline;
					material->shader = std::make_unique<ShaderFile>(file);
					material->updateProperties(true);

					materials.push_back(std::unique_ptr<Material>(material));
					pipelines.push_back(std::unique_ptr<Pipeline>(pipeline));
				}
			}
		}
	}
}
