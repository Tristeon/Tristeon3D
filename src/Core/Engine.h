#pragma once
#include <memory>

#include <Core/Managers/InputManager.h>
#include <Core/Rendering/Window.h>
#include <Core/Rendering/RenderManager.h>
#include <Scenes/SceneManager.h>
#include <Core/Components/ComponentManager.h>

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief Engine is the main engine class which is responsible for the creation of engine subsystems.
		 * 
		 * This class is not intended to be accessed or used by users.
		 */
		class Engine
		{
		public:
			Engine();
			/**
			 * \brief Starts the main engine loop. 
			 * Warning: This function starts an (almost) infinite loop. As such it only returns once the Engine closes.
			 */
			void run() const;

		private:
			std::unique_ptr<Rendering::RenderManager> renderSys;
			std::unique_ptr<Scenes::SceneManager> sceneSys;
			std::unique_ptr<Rendering::Window> window;
			std::unique_ptr<Components::ComponentManager> componentSys;
			std::unique_ptr<Managers::InputManager> inputSys;
		};
	}
}
