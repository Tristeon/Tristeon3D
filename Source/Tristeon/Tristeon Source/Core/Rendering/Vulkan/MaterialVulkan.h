#pragma once
#include "Core/Rendering/Material.h"
#include <vulkan/vulkan.hpp>
#include "Misc/Property.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				//Forward decl
				class InternalMeshRenderer;
				class Pipeline;
				class DebugDrawManager;
				//UBO decl
				/**
				 * \brief The uniform buffer object is used to send matrix information to the gpu.
				 */
				struct UniformBufferObject
				{
					/**
					 * \brief The transformation of the object
					 */
					glm::mat4 model;
					/**
					 * \brief The view matrix of the camera
					 */
					glm::mat4 view;
					/**
					 * \brief The projection matrix of the camera
					 */
					glm::mat4 proj;
				};

				/**
				 * \brief Vulkan implementation of Material. Controls the appearance of renderers in the world
				 */
				class Material : public Rendering::Material
				{
					friend class EditorGrid;
					friend InternalMeshRenderer;
					friend RenderManager;
					friend DebugDrawManager;

				public:
					/**
					 * \brief Cleans up all the resources allocated by Material
					 */
					~Material();

					SetProperty(diffuse) override;
				protected:
					/**
					 * \brief Sends UBO data to the shader
					 * \param model The model matrix of the current object
					 * \param view The view matrix of the current camera
					 * \param proj The projection matrix of the current camera
					 */
					void render(glm::mat4 model, glm::mat4 view, glm::mat4 proj) override;
					/**
					 * \brief Creates the vulkan texture data for the images
					 */
					void setupTextures() override;

					/**
					 * \brief Sets the active uniform buffer memory, used for rendering
					 * \param uniformBuffer The newly active memory
					 */
					void setActiveUniformBufferMemory(vk::DeviceMemory uniformBuffer);
				private:
					/**
					 * \brief The active uniform buffer memory
					 */
					vk::DeviceMemory uniformBufferMem;

					/**
					 * \brief Cleans up all the resources allocated by Material
					 */
					void cleanup() const;
					/**
					 * \brief Creates the descriptor sets for the textures
					 */
					void createDescriptorSets();
					/**
					 * \brief Creates the vulkan image for the material's textures
					 */
					void createTextureImage();
					/**
					 * \brief Creates the vulkan image view for the material's textures
					 */
					void createTextureImageView();
					/**
					 * \brief Creates the vulkan sampler for the material's textures
					 */
					void createTextureSampler();

					/**
					 * \brief The lookup ID of the assigned pipeline
					 */
					int pipelineID = 0;
					/**
					 * \brief The pipeline defining the shaders of this material
					 */
					Pipeline* pipeline = nullptr;

					/**
					 * \brief The descriptorset describing the binding for the material's textures
					 */
					vk::DescriptorSet set;

					/**
					 * \brief The vulkan image for the diffuse texture
					 */
					vk::Image diffuseImg;
					/**
					 * \brief The memory for the diffuse texture
					 */
					vk::DeviceMemory diffuseMemory;
					/**
					 * \brief The image view for the diffuse texture
					 */
					vk::ImageView diffuseView;
					/**
					 * \brief The image sampler for the diffuse texture
					 */
					vk::Sampler diffuseSampler;

					REGISTER_TYPE_H(Material)
				};
			}
		}
	}
}
