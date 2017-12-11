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
			REGISTER_TYPE_H(MaterialFileItem)
		};
	}
}
