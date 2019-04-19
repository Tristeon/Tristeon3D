#pragma once
#include "Core/Rendering/DebugDrawManager.h"
#include "RenderManagerVulkan.h"
#include "Core/Rendering/Components/MeshRenderer.h"
#include "Core/Rendering/ShaderFile.h"
#include "MaterialVulkan.h"
#include "API/BufferVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			//Forward decl
			class RenderManager;

			namespace Vulkan
			{
				//Forward decl
				class Forward;
				class RenderManager;

				/**
				* \brief Vulkan specific implementation of the debug draw manager utility class. Used to draw primitive shapes.
				*/
				class DebugDrawManager : public Rendering::DebugDrawManager
				{
					friend Vulkan::RenderManager;
					friend Vulkan::Forward;
					friend Rendering::RenderManager;
				public:
					/**
					 * \brief Creates a new instance of DebugDrawManager with the required rendering references
					 * \param offscreenPass The offscreenpass to render to
					 */
					DebugDrawManager(vk::RenderPass offscreenPass);
				protected:
					/**
					 * \brief Renders the queued drawables
					 */
					void draw() override;
					
					/**
					 * \brief The renderdata struct, given to us by vkRenderManager
					 */
					RenderData* data = nullptr;

					/**
					 * \brief Destroys the debug draw manager and deallocates all the resources created by it
					 */
					virtual ~DebugDrawManager();

					/**
					* \brief Rebuilds the Renderer. Used when resizing the window
					* \param offscreenPass The offscreen renderpass that the grid should be rendered to
					*/
					void rebuild(vk::RenderPass offscreenPass);
					/**
					 * \brief Renders the drawables
					 */
					void render();
					/**
					 * \brief Creates a new vertex buffer in vertexBuffers[i] with the given mesh.
					 * \param mesh The mesh data that is to be sent to the GPU
					 * \param i The index of the vertex buffer
					 */
					void createVertexBuffer(Data::SubMesh mesh, int i);
					void addMaterial();
					void createDescriptorSets();

					vk::RenderPass offscreenPass;

					/**
					* \brief The shader file for line rendering
					*/
					ShaderFile file;

					/**
					 * \brief The secondary command buffer, used to render the drawables
					 */
					vk::CommandBuffer cmd;
					
					/**
					 * \brief The vertex buffers, every line gets their own buffer
					 */
					std::vector<std::unique_ptr<BufferVulkan>> vertexBuffers;
					/**
					 * \brief The memory bound to the vertex buffers
					 */
					std::vector<vk::DeviceMemory> vertexBuffersMemory;
				
					std::unique_ptr<BufferVulkan> uniformBuffer;

					vk::DescriptorSet set;
					
					std::vector<std::unique_ptr<Material>> materials;
					std::vector<std::unique_ptr<Pipeline>> pipelines;
				};
			}
		}
	}
}
