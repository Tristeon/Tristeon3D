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
					int i = 0;
					while (!drawList.empty())
					{
						if (i >= materials.size())
							addMaterial();
						Material* m = materials[i].get();

						secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipeline());

						vk::DescriptorSet sets[] = { set, m->set };
						secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipelineLayout(), 0, 2, sets, 0, nullptr);

						DebugMesh const drawElement = drawList.front();

						m->setColor("Color.color", drawElement.color);
						m->setActiveUniformBufferMemory(uniformBuffer->getDeviceMemory());
						m->render(model, data->view, data->projection);

						Data::SubMesh mesh;
						mesh.vertices = drawElement.vertices;

						if (i >= vertexBuffers.size())
							vertexBuffers.push_back(nullptr);
						createVertexBuffer(mesh, i);

						vk::DeviceSize offsets[1] = { 0 };
						vk::Buffer vertex = vertexBuffers[i]->getBuffer();
						secondary.bindVertexBuffers(0, 1, &vertex, offsets);

						//Line width
						secondary.setLineWidth(drawElement.width);

						//Draw
						secondary.draw((uint32_t)mesh.vertices.size(), 1, 0, 0);

						drawList.pop();

						i++;
					}

					//Stop secondary cmd buffer
					secondary.end();

					data->lastUsedSecondaryBuffer = cmd;
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

				void DebugDrawManager::createVertexBuffer(Data::SubMesh mesh, int i)
				{
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					BufferVulkan staging = BufferVulkan(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
					staging.copyFromData(mesh.vertices.data());

					if (!vertexBuffers[i])
						vertexBuffers[i] = std::make_unique<BufferVulkan>(size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
					vertexBuffers[i]->copyFromBuffer(staging.getBuffer(), VulkanBindingData::getInstance()->commandPool, VulkanBindingData::getInstance()->graphicsQueue);
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
