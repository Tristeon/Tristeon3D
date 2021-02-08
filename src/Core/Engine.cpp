#include "Engine.h"

#include <Core/BindingData.h>
#include <Core/MessageBus.h>
#include <Misc/Hardware/Time.h>

namespace Tristeon::Core
{
	Engine::Engine()
	{
		Misc::Console::write("[ENGINE] [INIT] Initializing engine variables");
		
		window = std::make_unique<Rendering::Window>();
		renderSys = std::make_unique<Rendering::RenderManager>();

		inputSys = std::make_unique<Managers::InputManager>();
		componentSys = std::make_unique<Components::ComponentManager>();
		sceneSys = std::make_unique<SceneManager>();

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
		double fixedUpdateTime = 0;
		int frames = 0;
		double time = 0;

		while (!glfwWindowShouldClose(binding_data.window))
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
				MessageBus::sendMessage(MT_RENDER);
				MessageBus::sendMessage(MT_POSTRENDER);

				MessageBus::sendMessage(MT_AFTERFRAME);
			}
		}
	}
}
