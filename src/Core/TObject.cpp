#include <Core/TObject.h>
#include "Misc/Console.h"
#include "Misc/StringUtils.h"

namespace Tristeon
{
	namespace Core
	{
		TObject::TObject()
		{
			instanceID = StringUtils::generateRandom(12);
		}

		std::string TObject::getInstanceID() const
		{
			return instanceID;
		}

		void TObject::print(std::string data)
		{
			Misc::Console::write(data);
		}
	}
}
