#pragma once
#ifdef TRISTEON_EDITOR
#include "AssetItem.h"
#include <Core/Rendering/Materials/PBRMaterial.h>
#include <Editor/EditorImage.h>

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief The assetitem for pbr materials
		 */
		class PBRMaterialFileItem : public AssetItem
		{
		public:
			void init(std::string name, FolderItem* folder, std::string extension) override;
			void drawOnInspector() override;
			nlohmann::json serialize() override;
		private:
			REGISTER_TYPE_H(PBRMaterialFileItem)

			std::string openColorPicker;

			std::map<Tristeon::Core::Rendering::PBRMaterial::TextureType, EditorImage> editorImages;
		};
	}
}

#endif