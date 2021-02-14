#include "MeshRenderer.h"
#include <filesystem>

#include "Core/BindingData.h"
#include "Data/Resources.h"

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
			{
				auto* baseMesh = Data::Resources::assetLoad<Data::Mesh>(meshFilePathValue);
				if (submeshIDValue > 0 && submeshIDValue < baseMesh->submeshes.size())
					mesh = baseMesh->submeshes[submeshIDValue];
			}
			else
				mesh = Data::SubMesh();
		}

		meshFilePath = meshFilePathValue;
		subMeshID = submeshIDValue;
	}

	void MeshRenderer::render()
	{
		if (!material.get() || !vertices || !indices)
			return;

		binding_data.offscreenBuffer.bindVertexBuffers(0, vertices->buffer, { 0 });
		binding_data.offscreenBuffer.bindIndexBuffer(indices->buffer, 0, vk::IndexType::eUint16);
		binding_data.offscreenBuffer.drawIndexed((uint32_t)_mesh.indices.size(), 1, 0, 0, 0);
	}

	void MeshRenderer::createBuffers()
	{
		vertices = Buffer::createOptimized(_mesh.vertices.data(), _mesh.vertices.size() * sizeof(Data::Vertex), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
		indices = Buffer::createOptimized(_mesh.indices.data(), _mesh.indices.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	}
}