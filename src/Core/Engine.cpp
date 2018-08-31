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
				VulkanBindingData* bindingData = VulkanBindingData::getInstance();

				//Init window + store window info in bindingdata
				window = std::make_unique<Rendering::Vulkan::Window>();
				window->init();
				bindingData->tristeonWindow = window.get();
				bindingData->window = window->window;

				renderSys = std::make_unique<Rendering::Vulkan::RenderManager>();
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
