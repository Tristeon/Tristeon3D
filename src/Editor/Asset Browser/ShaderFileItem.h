#pragma once

#ifdef TRISTEON_EDITOR

#include "AssetItem.h"
#include "Editor/json.hpp"
#include <Editor/TypeRegister.h>

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

#endif