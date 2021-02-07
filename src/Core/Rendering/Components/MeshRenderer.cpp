#include "MeshRenderer.h"
#include <Core/UserPrefs.h>
#include "Misc/Console.h"
#include <Core/Rendering/Vulkan/InternalMeshRendererVulkan.h>
#include "Data/MeshBatch.h"
#include <filesystem>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			REGISTER_TYPE_CPP(MeshRenderer)

			void MeshRenderer::initInternalRenderer()
			{
				//Select based on rendering api
				if (UserPrefs::getStringValue("RENDERAPI") == "VULKAN")
					renderer = new Vulkan::InternalMeshRenderer(this);
				else
					Misc::Console::error("Trying to create a MeshRenderer with unsupported rendering API");
			}

			nlohmann::json MeshRenderer::serialize()
			{
				nlohmann::json j;
				j["typeID"] = Type<MeshRenderer>::fullName();
				j["name"] = "MeshRenderer";
				j["meshPath"] = meshFilePath;
				j["subMeshID"] = subMeshID;
				j["materialPath"] = materialPath;
				return j;
			}

			void MeshRenderer::deserialize(nlohmann::json json)
			{
				const std::string meshFilePathValue = json["meshPath"];
				const unsigned int submeshIDValue = json["subMeshID"];

				if (meshFilePath != meshFilePathValue || subMeshID != submeshIDValue)
				{
					if (std::filesystem::exists(meshFilePathValue))
						mesh = Data::MeshBatch::getSubMesh(meshFilePathValue, submeshIDValue);
					else
						mesh = Data::SubMesh();
				}

				meshFilePath = meshFilePathValue;
				subMeshID = submeshIDValue;

				const std::string materialPathValue = json["materialPath"];
				if (materialPath != materialPathValue)
					material = RenderManager::getMaterial(materialPathValue);
				materialPath = materialPathValue;
			}
		}
	}
}
