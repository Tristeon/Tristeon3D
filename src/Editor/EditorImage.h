#pragma once
#if TRISTEON_EDITOR

#include <vulkan/vulkan.hpp>
#include "Data/Image.h"
#include "Core/BindingData.h"
#include "EditorWindow.h"

namespace Tristeon
{
	//Forward decl
	namespace Core { class BindingData; }

	namespace Editor
	{
		//TODO: Make editor image generic and allow for API overrides
		class EditorImage
		{
		public:
			/**
			 * \brief Creates a vulkan image and its descriptor sets. 
			 * EditorImage MUST be destroyed when the editor is destroyed, the application WILL crash if you don't.
			 * \param image The image attached
			 * \param data The rendering binding data
			 */
			EditorImage(Data::Image image, Core::BindingData* data);
			/**
			* \brief EditorImage creates a vulkan image and its descriptor sets.
			* EditorImage MUST be destroyed when the editor is destroyed, the application WILL crash if you don't.
			* \param filePath The image path
			* \param data The rendering binding data
			*/
			EditorImage(std::string filePath, Core::BindingData* data);

			/**
			 * \brief Cleans up all resources allocated by EditorImage
			 */
			~EditorImage();

			/**
			 * \return Returns an ImGui Texture ID that can be used in ImGui::Image calls 
			 */
			ImTextureID getTextureID() const;
		private:
			Data::Image image;
			/**
			* \brief The vulkan image
			*/
			vk::Image img;
			/**
			* \brief The memory
			*/
			vk::DeviceMemory mem;
			/**
			* \brief The image view
			*/
			vk::ImageView view;
			/**
			* \brief The image sampler
			*/
			vk::Sampler sampler;
			/**
			* \brief The descriptorset describing the binding for the material's textures
			*/
			vk::DescriptorSet set;

			/**
			 * \brief The vulkan rendering data
			 */
			Core::VulkanBindingData* binding;

			/**
			* \brief Creates the descriptor sets for the textures
			*/
			void createDescriptorSets();
			/**
			* \brief Creates the vulkan image for the textures
			*/
			void createTextureImage();
			/**
			* \brief Creates the vulkan image view for the textures
			*/
			void createTextureImageView();
			/**
			* \brief Creates the vulkan sampler for the textures
			*/
			void createTextureSampler();
		};
	}
}

#endif