#include "InternalMeshRendererVulkan.h"
#include "MaterialVulkan.h"

#include "Core/Transform.h"
#include "Core/BindingData.h"

#include "Misc/Console.h"
#include "Core/Rendering/Components/Renderer.h"
#include "Core/Rendering/Components/MeshRenderer.h"

#include "HelperClasses/VulkanBuffer.h"
#include "HelperClasses/Pipeline.h"
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				InternalMeshRenderer::InternalMeshRenderer(Renderer* renderer) : InternalRenderer(renderer)
				{
					//We expect a meshrenderer
					meshRenderer = dynamic_cast<MeshRenderer*>(renderer);
					Misc::Console::t_assert(meshRenderer != nullptr, "Vulkan Internal Mesh Renderer received renderer component that isn't a mesh renderer!");
				}

				InternalMeshRenderer::~InternalMeshRenderer()
				{
					//Cleanup
					vk->device.waitIdle();

					vk->device.destroyBuffer(vertexBuffer);
					vk->device.freeMemory(vertexBufferMemory);

					vk->device.destroyBuffer(indexBuffer);
					vk->device.freeMemory(indexBufferMemory);

					vk->device.destroyBuffer(uniformBuffer);
					vk->device.freeMemory(uniformBufferMemory);

					vk->device.freeDescriptorSets(vk->descriptorPool, 1, &set);
				}

				void InternalMeshRenderer::render()
				{
					if (meshRenderer->mesh.vertices.size() == 0 || meshRenderer->mesh.indices.size() == 0)
						return;
					if ((VkBuffer)vertexBuffer == VK_NULL_HANDLE || (VkBuffer)indexBuffer == VK_NULL_HANDLE)
					{
						Misc::Console::warning("Not rendering [" + meshRenderer->gameObject->name + "] because either the vertex or index buffer hasn't been set up!");
						return;
					}

					//Get our material, and render it with the meshrenderer's model matrix
					Rendering::Material* m = meshRenderer->material;
					glm::mat4 const model = meshRenderer->transform->getTransformationMatrix();

					Vulkan::Material* vkm = dynamic_cast<Vulkan::Material*>(m);
					if (vkm == nullptr)
						return;
					if ((VkDescriptorSet)set == VK_NULL_HANDLE || (VkDescriptorSet)vkm->set == VK_NULL_HANDLE)
					{
						Misc::Console::warning("Not rendering [" + meshRenderer->gameObject->name + "] because the material hasn't been set up!");
						return;
					}

					vkm->setActiveUniformBufferMemory(uniformBufferMemory);
					vkm->render(model, data->view, data->projection);

					//Start secondary cmd buffer
					const vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &data->inheritance);
					vk::CommandBuffer secondary = cmdBuffers[data->index];
					secondary.begin(beginInfo);

					//Viewport/scissor
					secondary.setViewport(0, 1, &data->viewport);
					secondary.setScissor(0, 1, &data->scissor);
					//Pipeline
					secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, vkm->pipeline->getPipeline());

					//Descriptor sets
					std::array<vk::DescriptorSet, 2> sets = { set, vkm->set };
					secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vkm->pipeline->getPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);

					//Vertex / index buffer
					vk::Buffer vertexBuffers[] = { vertexBuffer };
					vk::DeviceSize offsets[1] = { 0 };
					secondary.bindVertexBuffers(0, 1, vertexBuffers, offsets);
					secondary.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);

					//Line width
					secondary.setLineWidth(2);

					//Draw
					secondary.drawIndexed(meshRenderer->mesh.indices.size(), 1, 0, 0, 0);

					//Stop secondary cmd buffer
					secondary.end();

					data->lastUsedSecondaryBuffer = cmdBuffers[data->index];
				}

				void InternalMeshRenderer::init(BindingData* data)
				{
					//Store rendering data
					vk = dynamic_cast<VulkanBindingData*>(data);
					Misc::Console::t_assert(vk != nullptr, "Vulkan Internal mesh renderer received binding data that isn't made for vulkan!");

					//INIT
					createCommandBuffers();
					createUniformBuffer(vk);
					createDescriptorSets();
					createVertexBuffer(meshRenderer->mesh);
					createIndexBuffer(meshRenderer->mesh);
				}

				void InternalMeshRenderer::onMeshChange(Data::SubMesh mesh)
				{
					createVertexBuffer(mesh);
					createIndexBuffer(mesh);
				}

				void InternalMeshRenderer::createCommandBuffers()
				{
					//Create a command buffer for each framebuffer
					//TODO: Could this be changed to a single command buffer?
					cmdBuffers.resize(vk->swapchain->getFramebufferCount());

					//Allocate command buffers
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(vk->commandPool, vk::CommandBufferLevel::eSecondary, cmdBuffers.size());
					vk::Result const r = vk->device.allocateCommandBuffers(&alloc, cmdBuffers.data());
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate command buffers: " + to_string(r));
				}

				void InternalMeshRenderer::createVertexBuffer(Data::SubMesh mesh)
				{
					if ((VkBuffer)vertexBuffer != VK_NULL_HANDLE)
						vk->device.destroyBuffer(vertexBuffer);

					//Vulkan's buffers will be able to enable high performance memory when we're using a staging buffer instead of directly copying data to the gpu
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					//Create staging buffer
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(vk, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);
					
					//Upload data to staging buffer
					void* data;
					vk->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, mesh.vertices.data(), size);
					vk->device.unmapMemory(stagingMemory);

					//Create vertex buffer
					
					VulkanBuffer::createBuffer(vk, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

					//Copy from staging buffer to vertex buffer
					VulkanBuffer::copyBuffer(vk, staging, vertexBuffer, size);

					//Cleanup staging buffer
					vk->device.destroyBuffer(staging);
					vk->device.freeMemory(stagingMemory);
				}

				void InternalMeshRenderer::createIndexBuffer(Data::SubMesh mesh)
				{
					if ((VkBuffer)indexBuffer != VK_NULL_HANDLE)
						vk->device.destroyBuffer(indexBuffer);

					vk::DeviceSize const size = sizeof(uint16_t) * mesh.indices.size();
					if (size == 0)
						return;

					//Create staging buffer
					vk::Buffer staging;
					vk::DeviceMemory stagingMemory;
					VulkanBuffer::createBuffer(vk, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging, stagingMemory);
					
					//Upload data to staging buffer
					void* data;
					vk->device.mapMemory(stagingMemory, 0, size, {}, &data);
					memcpy(data, mesh.indices.data(), size);
					vk->device.unmapMemory(stagingMemory);

					//Create index buffer
					VulkanBuffer::createBuffer(vk, size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

					//Copy data from staging buffer to index buffer
					VulkanBuffer::copyBuffer(vk, staging, indexBuffer, size);

					//Cleanup staging buffer
					vk->device.destroyBuffer(staging);
					vk->device.freeMemory(stagingMemory);
				}

				void InternalMeshRenderer::createUniformBuffer(VulkanBindingData* binding)
				{
					//Create a uniform buffer of the size of UniformBufferObject
					VulkanBuffer::createBuffer(binding, sizeof(UniformBufferObject),
						vk::BufferUsageFlagBits::eUniformBuffer,
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
						uniformBuffer, uniformBufferMemory);
				}

				void InternalMeshRenderer::createDescriptorSets()
				{
					VulkanBindingData* binding = vk; //Easy reference for now

					//Create a temporary layout describing the uniform buffer input
					vk::DescriptorSetLayout layout;
					vk::DescriptorSetLayoutBinding const ubo = vk::DescriptorSetLayoutBinding(
						0, vk::DescriptorType::eUniformBuffer,
						1, vk::ShaderStageFlagBits::eVertex,
						nullptr);
					vk::DescriptorSetLayoutCreateInfo ci = vk::DescriptorSetLayoutCreateInfo({}, 1, &ubo);
					binding->device.createDescriptorSetLayout(&ci, nullptr, &layout);

					//Allocate
					vk::DescriptorSetAllocateInfo alloc = vk::DescriptorSetAllocateInfo(binding->descriptorPool, 1, &layout);
					vk::Result const r = binding->device.allocateDescriptorSets(&alloc, &set);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate descriptor set!");

					//Write info
					vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(uniformBuffer, 0, sizeof(UniformBufferObject));
					vk::WriteDescriptorSet const uboWrite = vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &buffer, nullptr);

					//Update descriptor with our new write info
					std::array<vk::WriteDescriptorSet, 1> write = { uboWrite };
					binding->device.updateDescriptorSets(write.size(), write.data(), 0, nullptr);

					binding->device.destroyDescriptorSetLayout(layout);
				}
			}
		}
	}
}