#pragma once
#ifdef TRISTEON_EDITOR
#include <vulkan/vulkan.hpp>
#include "Data/Image.h"
#include "EditorWindow.h"

namespace Tristeon
{
	namespace Editor
	{
		class EditorImage
		{
		public:
			/**
			 * Default constructor, invalid for usage
			 */
			EditorImage();
			/**
			 * \brief Creates a vulkan image and its descriptor sets. 
			 * \param pImage The image attached
			 * \param data The rendering binding data
			 */
			EditorImage(Data::Image* pImage);
			/**
			* \brief EditorImage creates a vulkan image and its descriptor sets.
			* \param pFilePath The image path
			*/
			EditorImage(const std::string& pFilePath);

			/**
			 * \brief Cleans up all resources allocated by EditorImage
			 */
			~EditorImage();

			/**
			 * \return Returns an ImGui Texture ID that can be used in ImGui::Image calls 
			 */
			ImTextureID getTextureID() const;
		private:
			Data::Image* image = nullptr;
			/**
			* \brief The descriptorset describing the binding for the material's textures
			*/
			vk::DescriptorSet set = nullptr;

			/**
			* \brief Creates the descriptor sets for the textures
			*/
			void createDescriptorSets();
		};
	}
}

#endif