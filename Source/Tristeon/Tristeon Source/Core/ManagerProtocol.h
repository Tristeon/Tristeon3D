#pragma once
#include "Misc/Delegate.h"
#include <map>
#include "Message.h"
#include "Components/ComponentManager.h"

namespace Tristeon
{
	//Forward decl
	namespace Scenes { class SceneManager; }

	namespace Core
	{
		//Forward decl
		class Engine;
		class BindingData;
		class Message;
		namespace Managers { class InputManager; }
		namespace Rendering { class RenderManager;  class Window; }

		/**
		 * ManagerProtocol owns all subsystems/managers of the engine.
		 * 
		 * The ManagerProtocol is an abstract message bus that doesn't implement any engine logic,
		 * but allows for subsystems to communicate by sending messages containing information back and forth.
		 * 
		 * The ManagerProtocol is designed in this way to allow subsystems to have minimal knowledge of each other's existence, 
		 * so that dependencies can be mitigated to a minimum
		 */
		class ManagerProtocol
		{
			friend Engine;

		public:
			/**
			 * \brief Sends a message to every subsystem that has subscribed to message.type
			 * \param message The message
			 */
			static void sendMessage(Message message);

			/**
			 * \brief Adds a function to the message callbacks based on the given message type
			 * \param type The type of message the given function should listen to
			 * \param f The callback function
			 */
			static void subscribeToMessage(MessageType type, std::function<void(Message)> f);

		private:
			static ManagerProtocol* instance;
			/**
			 * \brief Creates an instance of ManagerProtocol, does not create subsystems. Use init() to create subsystems
			 * \param data The managerprotocol uses the bindingdata to allow for general data to be shared between subsystems
			 */
			explicit ManagerProtocol(BindingData* data);

			/**
			 * \brief Destroys the instance of ManagerProtocol. Cleans up all the created subsystems
			 */
			~ManagerProtocol();

			/**
			 * \brief Initializes ManagerProtocol and creates the engine's subsystems
			 */
			static void init();

			/**
			 * \brief Starts the main rendering loop. 
			 */
			static void run();

			/**
			 * \brief Validates if the given messagetype has already been added to the callback system. If not, the type will be added
			 * \param type The type that is to be validated
			 */
			static void validateMessageType(MessageType type);

			/**
			 * \brief Initializes the window and rendermanager based on the user's preference of rendering API
			 */
			static void setupRenderAPI();

			/**
			 * \brief The input subsystem, manages all keyboard/mouse/joystick data
			 */
			Managers::InputManager* inputSys = nullptr;
			
			/**
			 * \brief The component subsystem manages all components
			 */
			Components::ComponentManager* componentSys = nullptr;
			
			/**
			 * \brief The rendering subsystem, manages all renderers and cameras, renders the scene when asked to do so
			 */
			Rendering::RenderManager* renderSys = nullptr;
			/**
			 * \brief The scene subsystem, manages scene loading/unloading
			 */
			Scenes::SceneManager* sceneSys = nullptr;
			/**
			 * \brief The window subsystem, creates a GLFW window that can be used by the rendering subsystem
			 */
			Rendering::Window* window = nullptr;

			/**
			 * \brief The message callback list, stored as a map/dictionary for easy access.
			 * Delegate is a custom class, wrapping around std::function
			 */
			std::map<MessageType, Misc::Delegate<Message>> messageCallbacks;
			/**
			 * \brief Shared information that can be accessed by all subsystems
			 */
			BindingData* data;
		};
	}
}
