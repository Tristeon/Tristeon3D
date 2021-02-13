#include "Skybox.h"
#include "Data/Mesh.h"
#include "Data/Resources.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(Skybox)

	Skybox::Skybox()
	{
		mesh = Data::Resources::assetLoad<Data::Mesh>("Files/Models/skybox_cube.obj")->submeshes[0];
	}

	nlohmann::json Skybox::serialize()
	{
		nlohmann::json j;
		j["typeID"] = Type<Skybox>::fullName();
		j["texture"] = texturePath;
		return j;
	}

	void Skybox::deserialize(nlohmann::json json)
	{
		std::string const val = json["texture"];
		if (val != texturePath)
		{
			isDirty = true;
			texturePath = val;
		}
	}
}
