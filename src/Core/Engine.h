#pragma once
#include "ManagerProtocol.h"
#include "BindingData.h"
#include "Misc/Property.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief The main engine class. Owns the manager protocol and allows for gameplay control
		 */
		class Engine
		{
		public:
			/**
			 * \brief Creates the manager protocol and binding data
			 */
			void init();
			/**
			 * \brief Runs the main engine loop. Does not start the game
			 */
			void run() const;
			/**
			 * \brief Starts the gameplay loop
			 */
			void startGame() const;
			/**
			 * \brief Stops the gameplay loop
			 */
			void stopGame() const;

			/**
			 * \brief Cleans up the bindingdata and the managerprotocol
			 */
			~Engine();

			/**
			* \brief The bindingdata contains information to be shared over the engine's subsystems and the editor
			*/
			ReadOnlyProperty(BindingData*, bindingData);
			GetProperty(bindingData) { return bind.get(); }

		private:
			/**
			 * \brief The bindingdata contains information to be shared over the engine's subsystems and the editor
			 */
			std::unique_ptr<BindingData> bind = nullptr;
		};
	}
}
