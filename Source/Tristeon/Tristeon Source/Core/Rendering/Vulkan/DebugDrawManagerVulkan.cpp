#include "DebugDrawManagerVulkan.h"
#include "InternalMeshRendererVulkan.h"
#include "MaterialVulkan.h"
#include "Core/BindingData.h"
#include "HelperClasses/Pipeline.h"
#include "HelperClasses/VulkanBuffer.h"
#include "Core/Components/Camera.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				void DebugDrawManager::draw()
				{
					if (data == nullptr)
						return;
					if (drawList.size() == 0)
						return;

					material->render(glm::mat4(1.0f), data->view, data->projection);
					render();
				}

				DebugDrawManager::DebugDrawManager(VulkanBindingData* data, vk::RenderPass offscreenPass)
				{
					binding = data;

					//ShaderFile
					file = ShaderFile("Line", "Files/Shaders/", "LineV", "LineF", { });
					pipeline = new Pipeline(binding, file, binding->swapchain->extent2D, offscreenPass, file.getProperties(), true, vk::PrimitiveTopology::eLineList);

					material = new Vulkan::Material();
					material->pipeline = pipeline;
					material->shader = std::make_unique<ShaderFile>(file);
					material->updateProperties(true);

					//Allocate command buffers
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(binding->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					binding->device.allocateCommandBuffers(&alloc, &cmd);
				}

				DebugDrawManager::~DebugDrawManager()
				{
					delete material;
					delete pipeline;
					
					binding->device.freeCommandBuffers(binding->commandPool, cmd);

					for (const auto buf : vertexBuffers)
						binding->device.destroyBuffer(buf);
					for (const auto mem : vertexBuffersMemory)
						binding->device.freeMemory(mem);
				}

				void DebugDrawManager::rebuild(vk::RenderPass offscreenPass) const
				{
					pipeline->rebuild(binding->swapchain->extent2D, offscreenPass);
				}

				void DebugDrawManager::createVertexBuffer(Data::SubMesh mesh, int i)
				{
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					//Create staging buffer
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(binding, size, 
						vk::BufferUsageFlagBits::eTransferSrc, 
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);

					//Upload data to staging buffer
					void* data;
					binding->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, mesh.vertices.data(), size);
					binding->device.unmapMemory(stagingMemory);

					//Create vertex buffer
					if ((VkBuffer)vertexBuffers[i] == VK_NULL_HANDLE || (VkDeviceMemory)vertexBuffersMemory[i] == VK_NULL_HANDLE)
						VulkanBuffer::createBuffer(
							binding, size, 
							vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer, 
							vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffers[i], vertexBuffersMemory[i]);

					//Copy from staging buffer to vertex buffer
					VulkanBuffer::copyBuffer(binding, staging, vertexBuffers[i], size);

					//Cleanup staging buffer
					binding->device.destroyBuffer(staging);
					binding->device.freeMemory(stagingMemory);
				}

				void DebugDrawManager::render()
				{
					//Get our material, and render it with the meshrenderer's model matrix
					Material* m = material;
					glm::mat4 const model = glm::mat4(1.0f);
					m->render(model, data->view, data->projection);

					//Start secondary cmd buffer
					const vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(
						vk::CommandBufferUsageFlagBits::eRenderPassContinue, 
						&data->inheritance);
					vk::CommandBuffer secondary = cmd;
					secondary.begin(beginInfo);

					//Viewport/scissor
					secondary.setViewport(0, 1, &data->viewport);
					secondary.setScissor(0, 1, &data->scissor);
					//Pipeline
					secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipeline());

					//Descriptor sets
					secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m->pipeline->getPipelineLayout(), 0, 1, &m->set, 0, nullptr);

					int i = 0;
					while (!drawList.empty())
					{
						Line const l = drawList.front();

						Data::SubMesh mesh;
						mesh.vertices.push_back(l.start);
						mesh.vertices.push_back(l.end);

						if (i >= vertexBuffers.size())
						{
							vertexBuffers.push_back(nullptr);
							vertexBuffersMemory.push_back(nullptr);
						}
						createVertexBuffer(mesh, i);

						vk::DeviceSize offsets[1] = { 0 };
						secondary.bindVertexBuffers(0, 1, &vertexBuffers[i], offsets);

						//Line width
						secondary.setLineWidth(l.width);

						//Draw
						secondary.draw(mesh.vertices.size(), 1, 0, 0);

						drawList.pop();
						
						i++;
					}

					//Stop secondary cmd buffer
					secondary.end();

					data->lastUsedSecondaryBuffer = cmd;
				}
			}
		}
	}
}
