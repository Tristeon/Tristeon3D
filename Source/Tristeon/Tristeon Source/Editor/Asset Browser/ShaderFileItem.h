#pragma once
#include "AssetItem.h"

namespace Tristeon
{
	namespace Editor
	{
		class ShaderFileItem : public AssetItem
		{
		public:
			void drawOnInspector() override;
			nlohmann::json serialize() override;
		private:
			REGISTER_TYPE_H(ShaderFileItem)
		};
	}
}
