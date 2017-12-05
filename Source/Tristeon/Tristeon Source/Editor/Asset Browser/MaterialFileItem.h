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
			void drawOnInspector() override;
		};
	}
}
