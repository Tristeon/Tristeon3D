#pragma once
#include <string>
#include "Scene.h"

#ifdef TRISTEON_EDITOR
#include "Editor/Asset Browser/SceneFileitem.h"
#endif

namespace Tristeon
{
	namespace Scenes
	{
		class SceneManager final
		{
#ifdef TRISTEON_EDITOR
			friend class Tristeon::Editor::SceneFileItem;
#endif
		public:
			SceneManager();
			~SceneManager() { activeScene.reset(); }

			static void loadScene(int id);
			static void loadScene(std::string name);
			static void loadScene(Scene* scene);

			static Scene* getActiveScene() { return activeScene.get(); }
		
		private:
			static Core::Transform* findTransformByInstanceID(std::string instanceID);
			static void createParentalBonds(Scene* scene);

			static void addScenePath(std::string name, std::string path);
			static void removeScenePath(std::string name) { sceneFilePaths.erase(name); }

			static std::map<std::string,std::string> sceneFilePaths;

			static std::unique_ptr<Scene> activeScene;
		};
	}
}
