#pragma once
#include <string>
#include "Editor/Serializable.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * \brief TObject is the base class of all Tristeon classes. It implements some basic behaviour
		 */
		class TObject : public Serializable
		{
			friend class Transform;
			friend class GameObject;
		public:
			TObject();

			/**
			 * \brief The name of the Object
			 */
			std::string name;
			
			/**
			 * \brief Gets the instance ID of this object
			 * \return Returns the instance ID
			 */
			std::string getInstanceID() const;

			/**
			 * Prints the given data to the console. Only in Debug/ReleaseDebug/Editor.
			 */
			static void print(std::string data);

		private:
			/**
			* The instance id of the object.
			*/
			std::string instanceID;
		};
	}
}
