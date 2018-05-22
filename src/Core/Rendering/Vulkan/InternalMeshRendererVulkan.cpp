#include "InternalMeshRendererVulkan.h"
#include "MaterialVulkan.h"

#include "Core/Transform.h"
#include "Core/BindingData.h"

#include "Misc/Console.h"
#include "Core/Rendering/Components/Renderer.h"
#include "Core/Rendering/Components/MeshRenderer.h"

#include "HelperClasses/Pipeline.h"
#include "Core/GameObject.h"
#include "API/BufferVulkan.h"

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
					vk->device.freeDescriptorSets(vk->descriptorPool, 1, &set);
				}

				void InternalMeshRenderer::render()
				{
					if (meshRenderer->mesh.get().vertices.size() == 0 || meshRenderer->mesh.get().indices.size() == 0)
						return;
					if ((VkBuffer)vertexBuffer->getBuffer() == VK_NULL_HANDLE || (VkBuffer)indexBuffer->getBuffer() == VK_NULL_HANDLE)
					{
						Misc::Console::warning("Not rendering [" + meshRenderer->gameObject.get()->name + "] because either the vertex or index buffer hasn't been set up!");
						return;
					}

					//Get our material, and render it with the meshrenderer's model matrix
					Rendering::Material* m = meshRenderer->material.get();
					glm::mat4 const model = meshRenderer->transform.get()->getTransformationMatrix();

					Vulkan::Material* vkm = dynamic_cast<Vulkan::Material*>(m);
					if (vkm == nullptr)
						return;
					if ((VkDescriptorSet)set == VK_NULL_HANDLE || (VkDescriptorSet)vkm->set == VK_NULL_HANDLE)
						return;

					vkm->setActiveUniformBufferMemory(uniformBuffer->getDeviceMemory());
					vkm->render(model, data->view, data->projection);

					//Start secondary cmd buffer
					const vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &data->inheritance);
					vk::CommandBuffer secondary = cmd;
					secondary.begin(beginInfo);

					//Viewport/scissor
					secondary.setViewport(0, 1, &data->viewport);
					secondary.setScissor(0, 1, &data->scissor);
					//Pipeline
					secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, vkm->pipeline->getPipeline());

					//Descriptor sets
					std::vector<vk::DescriptorSet> sets = { set, vkm->set };
					if (data->skyboxSet && vkm->pipeline->getEnableLighting())
						sets.push_back(data->skyboxSet);

					secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vkm->pipeline->getPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);

					//Vertex / index buffer
					vk::Buffer vertexBuffers[] = { vertexBuffer->getBuffer() };
					vk::DeviceSize offsets[1] = { 0 };
					secondary.bindVertexBuffers(0, 1, vertexBuffers, offsets);
					secondary.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint16);

					//Line width
					secondary.setLineWidth(2);

					//Draw
					secondary.drawIndexed(meshRenderer->mesh.get().indices.size(), 1, 0, 0, 0);

					//Stop secondary cmd buffer
					secondary.end();

					data->lastUsedSecondaryBuffer = cmd;
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
					createVertexBuffer(meshRenderer->mesh.get());
					createIndexBuffer(meshRenderer->mesh.get());
				}

				void InternalMeshRenderer::onMeshChange(Data::SubMesh mesh)
				{
					createVertexBuffer(mesh);
					createIndexBuffer(mesh);
				}

				void InternalMeshRenderer::createCommandBuffers()
				{
					vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(vk->commandPool, vk::CommandBufferLevel::eSecondary, 1);
					vk::Result const r = vk->device.allocateCommandBuffers(&alloc, &cmd);
					Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate command buffers: " + to_string(r));
				}

				void InternalMeshRenderer::createVertexBuffer(Data::SubMesh mesh)
				{
					vk::DeviceSize const size = sizeof(Data::Vertex) * mesh.vertices.size();
					if (size == 0)
						return;

					vertexBuffer = BufferVulkan::createOptimized(vk, size, mesh.vertices.data(), vk::BufferUsageFlagBits::eVertexBuffer);
				}

				void InternalMeshRenderer::createIndexBuffer(Data::SubMesh mesh)
				{
					vk::DeviceSize const size = sizeof(uint16_t) * mesh.indices.size();
					if (size == 0)
						return;
					indexBuffer = BufferVulkan::createOptimized(vk, size, mesh.indices.data(), vk::BufferUsageFlagBits::eIndexBuffer);
				}

				void InternalMeshRenderer::createUniformBuffer(VulkanBindingData* binding)
				{
					uniformBuffer = std::make_unique<BufferVulkan>(binding, sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, 
						vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
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
					vk::DescriptorBufferInfo buffer = vk::DescriptorBufferInfo(uniformBuffer->getBuffer(), 0, sizeof(UniformBufferObject));
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
