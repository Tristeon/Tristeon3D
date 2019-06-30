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
#include "Misc/Hardware/Time.h"

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
				Misc::Console::error(api + " is not supported as a rendering API!");

			inputSys = std::make_unique<Managers::InputManager>(window->window);
			componentSys = std::make_unique<Components::ComponentManager>();
			sceneSys = std::make_unique<Scenes::SceneManager>();
			physicsSys = std::make_unique<Physics::Physics>();

			MessageBus::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg)
			{
				MessageBus::sendMessage(MT_START);
				inPlayMode = true;
			});
			MessageBus::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });
		}

		void Engine::run() const
		{
			double lastTime = glfwGetTime();
			float fixedUpdateTime = 0;
			int frames = 0;
			float time = 0;

			while (!glfwWindowShouldClose(window->window))
			{
				glfwPollEvents();

				//Keep track of elapsed time and frames and calculate FPS
				Misc::Time::deltaTime = glfwGetTime() - lastTime;
				lastTime = glfwGetTime();
				frames++;
				time += Misc::Time::deltaTime;
				if (time >= 1)
				{
					Misc::Time::fps = float(frames);
					frames = 0;
					time--;
				}

				Misc::Console::write("FPS: " + std::to_string(Misc::Time::fps));

				if (inPlayMode)
				{
					//FixedUpdate runs a fixed amount of times per second, the loop makes sure to catch up if we happen to be behind
					fixedUpdateTime += Misc::Time::deltaTime;
					while (fixedUpdateTime > 1.0f / 50.0f) //TODO: Make fixed delta time variable
					{
						MessageBus::sendMessage(MT_FIXEDUPDATE);
						fixedUpdateTime -= 1.0f / 50.0f;
					}

					MessageBus::sendMessage(MT_UPDATE);
					MessageBus::sendMessage(MT_LATEUPDATE);
				}

				//Only attempt to render if the window is a valid size
				if (window->width.get() != 0 && window->height.get() != 0)
				{
					MessageBus::sendMessage(MT_PRERENDER);
					MessageBus::sendMessage(MT_ONGUI);
					MessageBus::sendMessage(MT_RENDER);
					MessageBus::sendMessage(MT_POSTRENDER);

					MessageBus::sendMessage(MT_AFTERFRAME);
				}
			}
		}
	}
}