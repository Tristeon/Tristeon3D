#ifdef TRISTEON_EDITOR

#include "FileItem.h"

nlohmann::json Tristeon::Editor::FileItem::serialize()
{
	nlohmann::json output;
	output["typeID"] = Tristeon::Core::Type<FileItem>::fullName();
	output["isFolder"] = isFolder;
	output["filepath"] = filepath;
	output["name"] = name;
	output["GUID"] = GUID;
	return output;
}

void Tristeon::Editor::FileItem::deserialize(nlohmann::json json)
{
	isFolder = json["isFolder"];
	const std::string filepathValue = json["filepath"];
	filepath = filepathValue;
	const std::string nameValue = json["name"];
	name = nameValue;
	if (json["GUID"].is_string())
	{
		const std::string GUIDValue = json["GUID"];
		GUID = GUIDValue;
	}
}

#endif
