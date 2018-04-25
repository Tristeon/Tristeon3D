#include "MeshRenderer.h"
#include <Core/Settings.h>
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

			void MeshRenderer::initInternalRenderer(BindingData* data)
			{
				//Select based on rendering api
				switch (Settings::getRenderAPI())
				{
				case RAPI_Vulkan:
					//Vulkan mesh renderer
					renderer = new Vulkan::InternalMeshRenderer(this);
					break;
				default:
					Misc::Console::error("Trying to create a MeshRenderer with unsupported rendering API " + std::to_string(Settings::getRenderAPI()));
				}

				renderer->init(data);
			}

			nlohmann::json MeshRenderer::serialize()
			{
				nlohmann::json j;
				j["typeID"] = typeid(MeshRenderer).name();
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
					if (std::experimental::filesystem::exists(meshFilePathValue))
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