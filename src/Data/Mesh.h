#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Core/TObject.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include <vulkan/vulkan.hpp>

namespace Tristeon::Data
{
	/**
	 * The vertex struct describes the vertex and its data.
	 */
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		Vertex(const Math::Vector3& pos = Math::Vector3(0, 0, 0),
		                const Math::Vector3& normal = Math::Vector3(0, 0, 0),
		                const Math::Vector2& texCoord = Math::Vector2(0, 0)) : 
			pos({ pos.x, pos.y, pos.z }), normal({ normal.x, normal.y, normal.z }), texCoord({ texCoord.x, texCoord.y}) { /*Empty*/ }

		Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texCoord) : pos(pos), normal(normal), texCoord(texCoord) { /*Empty*/ }


		static std::array<vk::VertexInputAttributeDescription, 3> attributes()
		{
			std::array<vk::VertexInputAttributeDescription, 3> result;

			result[0] = vk::VertexInputAttributeDescription{ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) };
			result[1] = vk::VertexInputAttributeDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal) };
			result[2] = vk::VertexInputAttributeDescription{ 2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord) };

			return result;
		}

		static vk::VertexInputBindingDescription binding()
		{
			return vk::VertexInputBindingDescription{ 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
		}
	};

	/**
	 * A submesh struct defines an object that is part of a mesh file. 
	 */
	struct SubMesh
	{
		/**
		 * The vertices of this mesh
		 */
		std::vector<Vertex> vertices;
		/**
		 * The indices of this mesh
		 */
		std::vector<uint16_t> indices;
		/**
		 * The material ID. Temporary
		 */
		int materialID = 0;
	};

	struct Mesh : Core::TObject
	{
		Mesh() = default;
		explicit Mesh(std::string filePath);
		
		std::vector<SubMesh> submeshes;
	};
}