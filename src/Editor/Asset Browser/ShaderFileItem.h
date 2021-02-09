#pragma once
#ifdef TRISTEON_EDITOR

#include "AssetItem.h"
#include <json.hpp>
#include <Core/TypeRegister.h>

namespace Tristeon::Editor
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

#endif