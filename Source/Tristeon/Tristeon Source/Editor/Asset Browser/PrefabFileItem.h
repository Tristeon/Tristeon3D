#pragma once
#include "AssetItem.h"

/**
 * \brief PrefabFileItem is an asset item that stores gameobject data
 */
class PrefabFileItem : public Tristeon::Editor::AssetItem
{
public:
	void SetPrefabData(nlohmann::json prefabData) const;
	nlohmann::json GetPrefabData() const;

	nlohmann::json serialize() override;
private:
	static DerivedRegister<PrefabFileItem> reg;
};
