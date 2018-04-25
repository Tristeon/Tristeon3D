#pragma once
#include "Core/TObject.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class ManagerProtocol;

		namespace Managers
		{
			/**
			 * \brief The manager class defines some basic behavior that most engine subsystems use
			 */
			class Manager : public TObject
			{
			public:
				/**
				 * \brief "Initializes the manager". Default behavior subscribes the reset function but can be overriden.
				 */
				virtual void init();
			protected:
				/**
				 * \brief Every manager is required to describe its reset behavior. Reset generally gets called on scene destruction and when the application closes.
				 */
				virtual void reset() = 0;
			};
		}
	}
}