#pragma once
#include "Core/Rendering/Internal/InternalRenderer.h"
#include "RenderManagerVulkan.h"
#include <vulkan/vulkan.hpp>
#include "API/BufferVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			//Forward decl
			class MeshRenderer;

			namespace Vulkan
			{
				//Forward decl
				class Forward;

				/**
				 * \brief InternalMeshRenderer is the Vulkan implementation for the internal renderer for the meshrenderer class
				 */
				class InternalMeshRenderer : public InternalRenderer
				{
					friend Forward;
					friend RenderManager;

				public:
					/**
					 * \brief Creates a new instance of InternalMeshRenderer
					 * \param renderer The renderer that owns this object
					 */
					explicit InternalMeshRenderer(Renderer* renderer);
					/**
					 * \brief Frees all the memory that has been allocated by this instance
					 */
					~InternalMeshRenderer() override;
					/**
					 * \brief Renders the mesh data using the allocated command buffer
					 */
					void render() override;

					/**
					 * \brief Creates the command, index and vertex buffers
					 * \param data Global binding data
					 */
					void init(BindingData* data) override;

					/**
					* \brief Callback function for when the mesh has been changed
					* \param mesh The new mesh
					*/
					void onMeshChange(Data::SubMesh mesh) override;
				private:
					/**
					* \brief Creates the uniform buffer, used for passing uniform data to the shaders
					*/
					void createUniformBuffer(VulkanBindingData* binding);
					/**
					* \brief Creates the descriptor sets for the textures
					*/
					void createDescriptorSets();
					/**
					* \brief The descriptorset describing the binding for the material's textures
					*/
					vk::DescriptorSet set;

					/**
					 * \brief Rendering data, set by the renderer
					 */
					RenderData* data = nullptr;

					/**
					 * \brief The mesh renderer this internal renderer is attached to
					 */
					MeshRenderer* meshRenderer;

					/**
					 * \brief A reference to the binding data class
					 */
					VulkanBindingData* vk = nullptr;
					/**
					 * \brief The command buffers
					 */
					std::vector<vk::CommandBuffer> cmdBuffers;

					std::unique_ptr<BufferVulkan> vertexBuffer;
					std::unique_ptr<BufferVulkan> indexBuffer;
					std::unique_ptr<BufferVulkan> uniformBuffer;

					/**
					 * \brief Allocates the command buffers
					 */
					void createCommandBuffers();
					/**
					 * \brief Creates the vertex buffer and its memory
					 */
					void createVertexBuffer(Data::SubMesh mesh);
					/**
					 * \brief Creates the index buffer and its memory
					 */
					void createIndexBuffer(Data::SubMesh mesh);
				};
			}
		}
	}
}
