#pragma once
#include "AssetItem.h"
#include "Core/GameObject.h"

namespace Tristeon {
	namespace Editor {
		class PrefabFileItem : public AssetItem
		{
		public:
			void CreatePrefab(Core::GameObject gameObject);
		};
	}
}
