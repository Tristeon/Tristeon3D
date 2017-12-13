#pragma once
#include "AssetItem.h"
#include "Core/Rendering/Material.h"

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief The assetitem for materials
		 */
		class MaterialFileItem : public AssetItem
		{
		public:
			void drawOnInspector() override;
			nlohmann::json serialize() override;
		private:
			void drawProperty(Core::Rendering::Material* mat, Core::Rendering::ShaderProperty prop, std::string parent);
			REGISTER_TYPE_H(MaterialFileItem)

			std::string openColorPicker = "";
		};
	}
}
