#include "Engine.h"
#include "Settings.h"
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		void Engine::init()
		{
			//Create new binding data based on the selected API
			switch(Settings::getRenderAPI())
			{
			case Rendering::RAPI_Vulkan: 
				bind = std::make_unique<VulkanBindingData>();
				break;
			default: 
				Misc::Console::error("Trying to create bindingdata for unsupported graphics API!");
			}

			//Create protocol
			ManagerProtocol::instance = new ManagerProtocol(bindingData);
			ManagerProtocol::init();
		}

		void Engine::run() const
		{
			ManagerProtocol::run();
		}

		void Engine::startGame() const
		{
			ManagerProtocol::sendMessage(MT_GAME_LOGIC_START);
		}

		void Engine::stopGame() const
		{
			ManagerProtocol::sendMessage(MT_GAME_LOGIC_STOP);
		}

		Engine::~Engine()
		{
			//Cleanup
			delete ManagerProtocol::instance;
			ManagerProtocol::instance = nullptr;
		}
	}
}
