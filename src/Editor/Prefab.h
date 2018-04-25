#pragma once
#if TRISTEON_EDITOR

#include "Core/TObject.h"
#include "Asset Browser/PrefabFileItem.h"

namespace Tristeon {
	class Prefab : public Core::TObject
	{
	public:
		nlohmann::json getPrefabData();

		nlohmann::json serialize() override;

		void deserialize(nlohmann::json json) override;
	private:
		PrefabFileItem* prefabFile;
		std::string prefabFilePath;
	};
}

#endif