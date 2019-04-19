#pragma once
#include "Core/Rendering/DebugDrawManager.h"
#include "Core/Rendering/ShaderFile.h"
#include "MaterialVulkan.h"
#include "API/BufferVulkan.h"
#include "RenderManagerVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			class RenderManager;

			namespace Vulkan
			{
				class Forward;
				class RenderManager;

				/**
				* Vulkan specific implementation of the debug draw manager utility class. Used to draw primitive shapes.
				*/
				class DebugDrawManager : public Rendering::DebugDrawManager
				{
					friend RenderManager;
					friend Forward;
					friend Rendering::RenderManager;
				public:
					/**
					 * \brief Creates a new instance of DebugDrawManager with the required rendering references
					 * \param offscreenPass The offscreenpass to render to
					 */
					DebugDrawManager(vk::RenderPass offscreenPass);
				protected:
					virtual ~DebugDrawManager();

					/**
					 * Renders the queued drawables and clears the list
					 */
					void render() override;

					/**
					* Rebuilds the Renderer when the window is resized (specifically the vulkan pipelines)
					* \param offscreenPass The offscreen renderpass that the grid should be rendered to
					*/
					void onResize(vk::RenderPass offscreenPass);

					/**
					 * Creates a new vertex buffer in vertexBuffers[i] with the given mesh. Removes the old vertexbuffer at vertexBuffers[i].
					 * \param mesh The mesh data that is to be sent to the GPU
					 * \param i The index of the vertex buffer
					 */
					void createVertexBuffer(Data::SubMesh mesh, int i);
					void addMaterial();
					void createDescriptorSets();

					/**
					 * The renderdata struct. Passed as a reference by RenderManager, used to send data back/forth.
					 */
					RenderData* data = nullptr;
					vk::RenderPass offscreenPass;
				
					vk::CommandBuffer cmd;
					std::vector<std::unique_ptr<BufferVulkan>> vertexBuffers;
					std::unique_ptr<BufferVulkan> uniformBuffer;

					ShaderFile file;
					vk::DescriptorSet set;
					std::vector<std::unique_ptr<Material>> materials;
					std::vector<std::unique_ptr<Pipeline>> pipelines;
				};
			}
		}
	}
}
