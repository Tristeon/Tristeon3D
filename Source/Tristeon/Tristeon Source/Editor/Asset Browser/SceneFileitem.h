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
			 * \brief The event that is called when a fileitem in the assetbrowser is double clicked
			 */
			void onDoubleClick() override;

			REGISTER_TYPE_H(SceneFileItem)
		};
	}
}
