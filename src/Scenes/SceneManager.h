#pragma once
#include <string>
#include "Scene.h"

#ifdef TRISTEON_EDITOR
#include "Editor/Asset Browser/SceneFileitem.h"
#endif

namespace Tristeon
{
	namespace Core {
		class Engine;
	}
	namespace Scenes
	{
		/**
		 * SceneManager is a static class that provides scene loading/unloading functionality at runtime.
		 */
		class SceneManager final
		{
#ifdef TRISTEON_EDITOR
			friend Tristeon::Editor::SceneFileItem;
#endif
		public:
			SceneManager();
			~SceneManager() { activeScene.reset(); }
			
			/**
			 * Loads a scene based on the build ID, and unloads the previously loaded scene.
			 * The build ID is defined in the build settings for the project, and can be modified through
			 * the editor.
			 * If it fails to load a scene, it will still unload the old scene and leave an empty scene behind.
			 */
			static void loadScene(int id);
			/**
			 * Loads a scene based on the given scene name, and unloads the previously loaded scene.
			 * It will always use the first scene with the given name, which it will find using a lookup map.
			 * If it fails to load a scene, it will still unload the old scene and leave an empty scene behind.
			 */
			static void loadScene(std::string name);
			/**
			 * Loads the given scene.
			 */
			static void loadScene(Scene* scene);

			/**
			 * The current active scene. This value will never be null after engine initialization.
			 */
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
