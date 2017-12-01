#pragma once
#include <memory>
#include "Core/Rendering/ShaderFile.h"

namespace vk { class RenderPass; }

namespace Tristeon
{
	namespace Core
	{
		class VulkanBindingData;
		class GameObject;

		namespace Rendering
		{
			class MeshRenderer;

			namespace Vulkan
			{
				class Pipeline;
				class InternalMeshRenderer;
				class Material;

				/**
				 * \brief The EditorGrid is a class that manages the creation of a grid that is rendered in edit mode.
				 * EditorGrid uses several scene objects (GameObject, MeshRenderer), but all of these objects are unregistered and loading scenes won't affect their existence
				 */
				class EditorGrid
				{
				public:
					/**
					 * \brief Creates a new instance of EditorGrid and sets up all its resources
					 * \param data Binding data for rendering references 
					 * \param offscreenPass The offscreen renderpass that the grid should be rendered to
					 */
					EditorGrid(VulkanBindingData* data, vk::RenderPass offscreenPass);
					/**
					 * \brief Cleans up all the resources created by EditorGrid
					 */
					~EditorGrid();
					/**
					 * \brief Rebuilds the EditorGrid. Used when resizing the window
					 * \param offscreenPass The offscreen renderpass that the grid should be rendered to
					 */
					void rebuild(vk::RenderPass offscreenPass) const;

					/**
					 * \brief The internal grid renderer
					 */
					InternalMeshRenderer* renderer = nullptr;
					/**
					 * \brief The mesh renderer for vertex/index buffer allocation
					 */
					std::shared_ptr<MeshRenderer> mr = nullptr;
					/**
					 * \brief The shader pipeline
					 */
					Pipeline* pipeline = nullptr;
					/**
					 * \brief The material for the ubo data
					 */
					Material* material = nullptr;
					/**
					 * \brief The gameobject that owns the meshrenderer. 
					 */
					GameObject* object = nullptr;
					/**
					 * \brief The shader file for line rendering
					 */
					ShaderFile file;

				private:
					/**
					 * \brief A reference to VulkanBindingData to get rendering information
					 */
					VulkanBindingData* data;
				};
			}
		}
	}
}
