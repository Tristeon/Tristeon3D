#include "Skybox.h"
#include "Data/Mesh.h"
#include "Data/MeshBatch.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			REGISTER_TYPE_CPP(Skybox)

			Skybox::Skybox()
			{
				mesh = Data::MeshBatch::getSubMesh("Files/Models/skybox_cube.obj", 0);
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
	}
}
