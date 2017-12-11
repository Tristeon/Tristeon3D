#pragma once
#include "Core/Rendering/Material.h"
#include <vulkan/vulkan.hpp>
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
				 * \brief The texture struct wraps around all the vulkan objects that are
				 * required to pass a texture onto the shader.
				 */
				struct Texture
				{
					/**
					 * \brief The vulkan image, used to bind everything together
					 */
					vk::Image img = nullptr;
					/**
					 * \brief The allocated GPU memory, used to send the image 
					 * data to the GPU.
					 */
					vk::DeviceMemory mem = nullptr;
					/**
					 * \brief The image view represents a way for shaders to read/write data to the image, instead
					 * of modifying the image directly.
					 */
					vk::ImageView view = nullptr;
					/**
					 * \brief The image sampler is used to pass the image onto the GPU, while applying filtering
					 */
					vk::Sampler sampler = nullptr;
				};

				/**
				 * \brief The uniform buffer struct wraps around the required vulkan objects
				 * to create a uniform buffer, that is used to send uniform data to the GPU.
				 */
				struct UniformBuffer
				{
					/**
					 * \brief The buffer. Used to bind the buffer memory to the shader
					 */
					vk::Buffer buf;
					/**
					 * \brief The buffer memory. Used to send uniform data to the GPU
					 */
					vk::DeviceMemory mem;
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

					/**
					* \brief Sets the texture property with the name [name] to the given Image. Will update the vulkan resource data.
					* \param name The name of the texture property. The function will return a warning if this name hasn't been registered.
					* \param path The path to the new texture
					*/
					void setTexture(std::string name, std::string path) override;
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

					/**
					 * \brief Updates the resource data based on the current shader.
					 * \param updateResources If set to true, we'll also update the allocated vulkan resources 
					 */
					void updateProperties(bool updateResources) override;

					void updateShader() override;
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

					UniformBuffer createUniformBuffer(std::string name);
					/**
					 * \brief Creates the vulkan image for the material's textures
					 */
					void createTextureImage(Data::Image img, Texture& tex) const;
					/**
					 * \brief Creates the vulkan sampler for the material's textures
					 */
					void createTextureSampler(Texture& tex) const;

					/**
					 * \brief The pipeline defining the shaders of this material
					 */
					Pipeline* pipeline = nullptr;

					/**
					 * \brief The descriptorset describing the binding for the material's textures
					 */
					vk::DescriptorSet set;

					/**
					 * \brief The vulkan textures
					 */
					std::map<std::string, Texture> textures;
					/**
					 * \brief The uniform buffers for all non-texture properties
					 */
					std::map<std::string, UniformBuffer> uniformBuffers;

					REGISTER_TYPE_H(Vulkan::Material)
				};
			}
		}
	}
}
