#pragma once
#include <memory>

#include <Core/Managers/InputManager.h>
#include <Core/Rendering/Window.h>
#include <Core/Rendering/RenderManager.h>
#include <Core/SceneManager.h>
#include <Core/Components/ComponentManager.h>

namespace Tristeon::Core
{
	/**
	 * Engine is the main engine class which is responsible for the creation of engine subsystems.
	 * 
	 * This class is not intended to be accessed or used by users.
	 */
	class Engine final
	{
	public:
		Engine();
		/**
		 * Starts the main engine loop. 
		 * Warning: This function starts an (almost) infinite loop. As such it only returns once the Engine closes.
		 * 
		 * \exception runtime_error Unsupported rendering API requested
		 */
		void run() const;

	private:
		std::unique_ptr<Rendering::RenderManager> renderSys;
		std::unique_ptr<SceneManager> sceneSys;
		std::unique_ptr<Rendering::Window> window;
		std::unique_ptr<Components::ComponentManager> componentSys;
		std::unique_ptr<Managers::InputManager> inputSys;

		bool inPlayMode = false;
	};
}