#include "ManagerProtocol.h"

//Managers
#include "Rendering/Vulkan/WindowVulkan.h"
#include "Rendering/Vulkan/RenderManagerVulkan.h"
#include "Managers/InputManager.h"
#include "Rendering/RenderAPI.h"
#include "Scenes/SceneManager.h"

//Other
#include "Misc/Console.h"
#include "Settings.h"
#include "Message.h"

#include <Core/BindingData.h>

namespace Tristeon
{
	namespace Core
	{
		ManagerProtocol* ManagerProtocol::instance = nullptr;

		ManagerProtocol::ManagerProtocol(BindingData* data) : data(data)
		{
			//Empty
		}

		ManagerProtocol::~ManagerProtocol()
		{
			//Cleanup
			delete inputSys;
			delete componentSys;
			delete window;
			delete sceneSys;
			delete renderSys;
		}

		void ManagerProtocol::init()
		{
			//Get user preferences
			Settings::init();

			//Render API
			setupRenderAPI();

			//Input
			instance->inputSys = new Managers::InputManager(instance->window->window);
			instance->inputSys->init();

			instance->componentSys = new Components::ComponentManager();
			instance->componentSys->init();

			//Setup scene manager and load first scene
			instance->sceneSys = new Scenes::SceneManager();
			instance->sceneSys->init();
		}

		void ManagerProtocol::setupRenderAPI()
		{
			//Setup a render API based on the user's preferences
			switch (Settings::getRenderAPI())
			{
			case Rendering::RAPI_Vulkan:
				//Create vulkan window
				instance->window = new Rendering::Vulkan::Window();
				instance->window->init();

				//Store in bindingdata to allow others to know about our window. 
				instance->data->tristeonWindow = instance->window;
				instance->data->window = instance->window->window;

				//Create vulkan render manager
				instance->renderSys = new Rendering::Vulkan::RenderManager(instance->data);
				instance->renderSys->init();
				break;
			default: //Extendable to any other API, but currently only supports Vulkan 
				Misc::Console::error("Trying to load using unsupported rendering API: " + std::to_string(Settings::getRenderAPI()));
				break;
			}
		}

		void ManagerProtocol::run()
		{
			instance->window->mainLoop();
		}

		void ManagerProtocol::sendMessage(Message message)
		{
			validateMessageType(message.type);
			instance->messageCallbacks[message.type].invoke(message);
		}

		void ManagerProtocol::subscribeToMessage(MessageType type, std::function<void(Message)> f)
		{
			validateMessageType(type);
			instance->messageCallbacks[type] += f;
		}

		void ManagerProtocol::validateMessageType(MessageType type)
		{
			//Check for unknown message types
			if (!instance->messageCallbacks.count(type))
				instance->messageCallbacks[type] = Misc::Delegate<Message>();
		}
	}
}
