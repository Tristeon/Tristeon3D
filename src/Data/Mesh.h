#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Core/TObject.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include <vulkan/vulkan.hpp>

namespace Tristeon
{
	namespace Data
	{
		/**
		 * The vertex struct describes the vertex and its data.
		 */
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 texCoord;

			Vertex(Math::Vector3 pos = Math::Vector3(0, 0, 0), 
				   Math::Vector3 normal = Math::Vector3(0, 0, 0), 
				   Math::Vector2 texCoord = Math::Vector2(0, 0)) : 
				pos({ pos.x, pos.y, pos.z }), normal({ normal.x, normal.y, normal.z }), texCoord({ texCoord.x, texCoord.y}) { /*Empty*/ }

			Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoord) : pos(pos), normal(normal), texCoord(texCoord) { /*Empty*/ }


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
		 * A submesh struct defines a struct that is part of a mesh file. 
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

		/**
		 * Class for mesh handling
		 */
		struct Mesh : Core::TObject
		{
			/**
			 * The list of submeshes that this mesh exists of
			 */
			std::vector<SubMesh> submeshes;

			/**
			 * Returns a mesh object based on the given filepath
			 * \param filePath The filepath of the mesh file
			 * \return Returns a mesh object
			 */
			static Mesh fromFile(std::string filePath);

			/**
			 * Loads the mesh from a file and fills in the submeshes variable
			 * \param filePath 
			 * 
			 * \exception runtime_error Non-triangular shape detected 
			 */
			void load(std::string filePath);
		};
	}
}
