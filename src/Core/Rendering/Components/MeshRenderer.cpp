#include "MeshRenderer.h"
#include <Data/MeshBatch.h>
#include <filesystem>

#include "Core/BindingData.h"

namespace Tristeon::Core::Rendering
{
	REGISTER_TYPE_CPP(MeshRenderer);

	nlohmann::json MeshRenderer::serialize()
	{
		nlohmann::json j = Renderer::serialize();
		j["typeID"] = Type<MeshRenderer>::fullName();
		j["name"] = "MeshRenderer";
		j["meshPath"] = meshFilePath;
		j["subMeshID"] = subMeshID;
		j["materialPath"] = materialPath;
		return j;
	}

	void MeshRenderer::deserialize(nlohmann::json json)
	{
		Renderer::deserialize(json);
		
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
	}

	void MeshRenderer::render()
	{
		if (!material.get())
			return;
		
		binding_data.offscreenBuffer.draw(3, 1, 0, 0);
	}
}