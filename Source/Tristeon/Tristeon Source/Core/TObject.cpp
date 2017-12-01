#include <Core/TObject.h>
#include "Misc/Console.h"

namespace Tristeon
{
	namespace Core
	{
		int TObject::getInstanceID() const
		{
			return instanceID;
		}

		void TObject::print(std::string data)
		{
			Misc::Console::write(data);
		}
	}
}