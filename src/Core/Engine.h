#pragma once
#include "Misc/Property.h"
#include <memory>

#include <Core/Managers/InputManager.h>
#include <Core/Rendering/Window.h>
#include <Core/Rendering/RenderManager.h>
#include <Scenes/SceneManager.h>
#include <Core/Components/ComponentManager.h>
#include <Core/BindingData.h>
namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief The main engine class. Owns the manager protocol and allows for gameplay control
		 */
		class Engine
		{
		public:
			Engine();
			/**
			 * \brief Runs the main engine loop. Does not start the game
			 */
			void run() const;

			/**
			* \brief The bindingdata contains information to be shared over the engine's subsystems and the editor
			*/
			ReadOnlyProperty(Engine, bindingData, BindingData*);
			GetProperty(bindingData) { return bind.get(); }

		private:
			/**
			 * \brief The bindingdata contains information to be shared over the engine's subsystems and the editor
			 */
			std::unique_ptr<BindingData> bind {};

			std::unique_ptr<Rendering::RenderManager> renderSys = nullptr;
			std::unique_ptr<Scenes::SceneManager> sceneSys = nullptr;
			std::unique_ptr<Rendering::Window> window = nullptr;
			std::unique_ptr<Components::ComponentManager> componentSys = nullptr;
			std::unique_ptr<Managers::InputManager> inputSys = nullptr;
		};
	}
}
