#pragma once
#include "Core/Rendering/DebugDrawManager.h"
#include "RenderManagerVulkan.h"
#include "Core/Rendering/Components/MeshRenderer.h"
#include "Core/Rendering/ShaderFile.h"
#include "MaterialVulkan.h"

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
					 * \brief Creates a new instance of DebugDrawManager with the required rendering references
					 * \param data The binding data
					 * \param offscreenPass The offscreenpass to render to
					 */
					DebugDrawManager(VulkanBindingData* data, vk::RenderPass offscreenPass);
					/**
					 * \brief Destroys the debug draw manager and deallocates all the resources created by it
					 */
					virtual ~DebugDrawManager();

					/**
					* \brief Rebuilds the Renderer. Used when resizing the window
					* \param offscreenPass The offscreen renderpass that the grid should be rendered to
					*/
					void rebuild(vk::RenderPass offscreenPass) const;
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
					/**
					* \brief The shader pipeline
					*/
					Pipeline* pipeline = nullptr;
					/**
					* \brief The material for the ubo data
					*/
					Vulkan::Material* material = nullptr;
					/**
					* \brief The shader file for line rendering
					*/
					ShaderFile file;

					/**
					 * \brief The secondary command buffer, used to render the drawables
					 */
					vk::CommandBuffer cmd;
					
					/**
					* \brief A reference to VulkanBindingData to get rendering information
					*/
					VulkanBindingData* binding;

					/**
					 * \brief The vertex buffers, every line gets their own buffer
					 */
					std::vector<vk::Buffer> vertexBuffers;
					/**
					 * \brief The memory bound to the vertex buffers
					 */
					std::vector<vk::DeviceMemory> vertexBuffersMemory;
				
					/**
					* \brief The uniform buffer, used for passing uniform data to the shaders
					*/
					vk::Buffer uniformBuffer;
					/**
					* \brief The uniform buffer memory, containing the uniform data
					*/
					vk::DeviceMemory uniformBufferMemory;
					vk::DescriptorSet set;

					void createDescriptorSets();
				};
			}
		}
	}
}
