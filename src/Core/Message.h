#pragma once

namespace Tristeon
{
	namespace Core
	{
		class TObject;

		/**
		 * Describes a type of message
		 */
		enum MessageType
		{
			MT_START,
			MT_UPDATE,
			MT_LATEUPDATE,
			MT_FIXEDUPDATE,
			MT_PRERENDER,
			MT_RENDER,
			MT_POSTRENDER,
			MT_AFTERFRAME,
			MT_QUIT,

			MT_MANAGER_RESET,

			MT_RENDERINGCOMPONENT_REGISTER,
			MT_SCRIPTINGCOMPONENT_REGISTER,

			MT_CAMERA_REGISTER,
			MT_CAMERA_DEREGISTER,

			MT_RENDERINGCOMPONENT_DEREGISTER,
			MT_SCRIPTINGCOMPONENT_DEREGISTER,

			MT_GAME_LOGIC_START,
			MT_GAME_LOGIC_STOP,

			MT_WINDOW_RESIZE,

			MT_SHARE_DATA,
			MT_ONGUI,
		};

		/**
		 * The message class is used for sending messages back and forth through the managerprotocol.
		 */
		class Message
		{
		public:
			/**
			 * Creates a message with the given type. UserData will be nullptr
			 * \param type The type of the message
			 */
			Message(MessageType type);
			/**
			 * Creates a message with the given type and userdata
			 * \param type The type of the message
			 * \param userData The userdata to be sent with the message
			 */
			Message(MessageType type, TObject* userData);
			
			MessageType type;
			TObject* userData = nullptr;
		};
	}
}