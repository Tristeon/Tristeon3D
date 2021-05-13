#include "MeshRenderer.h"
#include <filesystem>

#include "Core/RenderData.h"
#include "Core/Engine.h"
#include "Core/Transform.h"
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

	void MeshRenderer::render(vk::CommandBuffer cmd, const uint8_t& frameIndex, glm::mat4 proj, glm::mat4 view)
	{
		if (!material.get() || !vertices || !indices)
			return;

		Transform::UniformBuffer ubo{ transform.get()->getTransformationMatrix(), view, proj };

		//Send data
		void* data;
		VULKAN_DEBUG(renderData.device.mapMemory(transformBuffer->memory, 0, sizeof(ubo), {}, &data));
		memcpy(data, &ubo, sizeof ubo);
		renderData.device.unmapMemory(transformBuffer->memory);

		std::array<vk::DescriptorSet, 2> sets{ transformSet, _material->set(frameIndex) };
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _material->layout(), 0, sets, {});

		cmd.bindVertexBuffers(0, vertices->buffer, { 0 });
		cmd.bindIndexBuffer(indices->buffer, 0, vk::IndexType::eUint16);
		cmd.drawIndexed((uint32_t)_mesh.indices.size(), 1, 0, 0, 0);
	}

	void MeshRenderer::createBuffers()
	{
		if (_mesh.vertices.empty() || _mesh.indices.empty())
		{
			vertices.reset();
			indices.reset();
			return;
		}
		
		vertices = Buffer::createOptimized(_mesh.vertices.data(), (uint32_t)_mesh.vertices.size() * sizeof(Data::Vertex), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
		indices = Buffer::createOptimized(_mesh.indices.data(), (uint32_t)_mesh.indices.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	}
}