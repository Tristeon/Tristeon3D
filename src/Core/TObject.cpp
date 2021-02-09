#include <Core/TObject.h>
#include <Misc/Console.h>
#include <Misc/StringUtils.h>

namespace Tristeon::Core
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

	nlohmann::json TObject::serialize()
	{
		nlohmann::json j = Serializable::serialize();
		j["instanceID"] = instanceID;
		return j;
	}

	void TObject::deserialize(nlohmann::json json)
	{
		Serializable::deserialize(json);
		instanceID = json.value("instanceID", instanceID);
	}
}
