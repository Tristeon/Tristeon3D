#pragma once
#if TRISTEON_EDITOR

#include "AssetItem.h"

namespace Tristeon {
	namespace Scenes {
		class Scene;
	}
}

namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief The assetitem for scenes
		 */
		class SceneFileItem : public AssetItem
		{
		public:
			SceneFileItem();

			void createFile(nlohmann::json json) override;

			/**
			 * \brief The event that is called when a fileitem in the assetbrowser is double clicked
			 */
			void onDoubleClick() override;

			void removeFile() override;

			nlohmann::json serialize() override;

			void deserialize(nlohmann::json json) override;

			REGISTER_TYPE_H(SceneFileItem)
		};
	}
}

#endif