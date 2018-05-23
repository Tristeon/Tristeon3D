#pragma once
#ifdef TRISTEON_EDITOR

#include "AssetItem.h"

namespace Tristeon
{
	namespace Editor
	{
		class SkyboxFileItem : public AssetItem
		{
		public:
			void drawOnInspector() override;
			nlohmann::json serialize() override;
		private:
			REGISTER_TYPE_H(SkyboxFileItem)
		};
	}
}

#endif