#include "Engine.h"
#include "BindingData.h"
#include "UserPrefs.h"
#include "Misc/Console.h"

#include "Rendering/Vulkan/WindowVulkan.h"
#include "Rendering/Vulkan/RenderManagerVulkan.h"
#include "Managers/InputManager.h"
#include "Components/ComponentManager.h"
#include "Scenes/SceneManager.h"
#include "MessageBus.h"

namespace Tristeon
{
	namespace Core
	{
		Engine::Engine()
		{
			UserPrefs::readPrefs();

			const std::string api = UserPrefs::getStringValue("RENDERAPI");

			if (api == "VULKAN")
			{
				bind = std::make_unique<VulkanBindingData>();

				//Create vulkan window
				window = std::make_unique<Rendering::Vulkan::Window>();
				window->init();

				//Store in bindingdata to allow others to know about our window. 
				bind->tristeonWindow = window.get();
				bind->window = window->window;

				//Create vulkan render manager
				renderSys = std::make_unique<Rendering::Vulkan::RenderManager>(bindingData);
			}
			else
				Misc::Console::error(api + " is not yet supported as a rendering API!");

			inputSys = std::make_unique<Managers::InputManager>(window->window);
			componentSys = std::make_unique<Components::ComponentManager>();
			sceneSys = std::make_unique<Scenes::SceneManager>();
		}

		void Engine::run() const
		{
			window->mainLoop();
		}
	}
}
