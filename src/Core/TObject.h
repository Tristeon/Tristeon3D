#pragma once
#include <string>
#include "Editor/Serializable.h"

namespace Tristeon
{
	namespace Core
	{
		/**
		 * TObject is the base class of all Tristeon classes.
		 * Every TObject contains a name and a instanceID. The instanceID is a unique string generated upon creation or loaded in through serialization.
		 */
		class TObject : public Serializable
		{
			friend class Transform;
			friend class GameObject;
		public:
			TObject();

			std::string name;
			std::string getInstanceID() const;

			/**
			 * Prints the given data to the console. Only in Debug/ReleaseDebug/Editor.
			 */
			static void print(std::string data);
		private:
			std::string instanceID;
		};
	}
}
