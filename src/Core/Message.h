#pragma once

namespace Tristeon::Core
{
	class TObject;

	/**
	 * The message class is used for sending messages back and forth through the managerprotocol.
	 */
	class Message
	{
	public:
		/**
		 * Describes a type of message
		 */
		enum class Type
		{
			Start,

			EarlyUpdate,
			Update,
			LateUpdate,
			FixedUpdate,
			PreRender,
			Render,
			PostRender,
			AfterFrame,

			GameLogicStart,
			GameLogicStop,

			SceneUnloaded,
			SceneLoaded,
			
			WindowResize,
			Quitting
		};
		
		/**
		 * Creates a message with the given type. UserData will be nullptr
		 * \param type The type of the message
		 */
		Message(Type type);
		/**
		 * Creates a message with the given type and userdata
		 * \param type The type of the message
		 * \param userData The userdata to be sent with the message
		 */
		Message(Type type, TObject* userData);

		Type type;
		TObject* userData = nullptr;
	};
}