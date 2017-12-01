#pragma once
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

			/**
			 * \brief Creates the actual scene file, can also be used for saving a scene
			 */
			void createSceneFile(Scenes::Scene scene);
			/**
			 * \brief The event that is called when a fileitem in the assetbrowser is double clicked
			 */
			void onDoubleClick() override;

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			REGISTER_TYPE_H(SceneFileItem)
		};
	}
}
