#pragma once
#include "Core/TObject.h"
#include <vector>
#include <memory>
#include "Core/GameObject.h"

namespace Tristeon
{
	namespace Scenes
	{
		/**
		 * Scenes contain everything inside of your level/game. From environments to characters, physics bodies etc.
		 * A scene object can exist without it being loaded in. If you wish to manually create a scene and load it in after, use SceneManager::loadScene(scene);
		 */
		class Scene final : public Core::TObject
		{
			friend SceneManager;
		public:
			/**
			 * Adds the GameObject to the scene and reserializes it to set its values back to their defaults.
			 * Required to pass a unique_ptr as Scene takes over ownership of the given GameObject.
			 * Will automatically initialize the GameObject if the scene's already loaded.
			 */
			void addGameObject(std::unique_ptr<Core::GameObject> gameObj);

			/**
			 * Removes the GameObject from the scene. 
			 * The GameObject automatically gets destroyed. 
			 * Any references to the GameObject will automatically turn invalid.
			 */
			void removeGameObject(Core::GameObject* gameObj);

			/**
			 * Returns the first GameObject in the list with the given instanceID. 
			 * Will return nullptr if no GO is found.
			 */
			Core::GameObject* getGameObject(std::string instanceID);

			Core::GameObject* getGameObjectByName(std::string name);

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
		private:
			void init();

			std::vector<std::unique_ptr<Tristeon::Core::GameObject>> gameObjects;
			REGISTER_TYPE_H(Scene)
		};
	}
}