#include "Mesh.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Misc/Console.h"

namespace Tristeon
{
	namespace Data
	{
		Mesh Mesh::fromFile(std::string filePath)
		{
			Mesh m;
			m.load(filePath);
			return m;
		}

		void Mesh::load(std::string filePath)
		{
			Assimp::Importer imp;

			//Postprocessing
			//TODO: Get rid of post processing and allow for file processing in the editor
			unsigned int postProcess =
				aiProcess_CalcTangentSpace |
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_GenUVCoords |
				aiProcess_GenSmoothNormals;

			postProcess |= aiProcess_FlipUVs; //Vulkan needs flipped UVs

			const auto scene = imp.ReadFile(filePath, postProcess);
			if (!scene)
				return;

			if (scene->HasMeshes())
			{
				for (size_t i = 0; i < scene->mNumMeshes; i++)
				{
					const auto currentMesh = scene->mMeshes[i];

					SubMesh submesh;
					for (size_t j = 0; j < currentMesh->mNumVertices; j++)
					{
						const auto position = currentMesh->mVertices[j];
						const auto normal = currentMesh->mNormals[j];
						const auto texCoord = currentMesh->mTextureCoords[0] ? currentMesh->mTextureCoords[0][j] : aiVector3D(0, 0, 0);

						//Create vertex and add to our list
						Vertex vertex;
						vertex.pos = glm::vec3(position.x, position.y, position.z);
						vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
						vertex.texCoord = glm::vec2(float(texCoord.x), float(texCoord.y));
						submesh.vertices.push_back(vertex);
					}

					for (size_t j = 0; j < currentMesh->mNumFaces; j++)
					{
						const auto face = currentMesh->mFaces[j];
						Misc::Console::t_assert(face.mNumIndices == 3, "Non-triangular shape detected inside mesh");
						submesh.indices.push_back(face.mIndices[0]);
						submesh.indices.push_back(face.mIndices[1]);
						submesh.indices.push_back(face.mIndices[2]);
					}

					//Set material (temporary)
					submesh.materialID = currentMesh->mMaterialIndex;
					submeshes.push_back(submesh);
				}
			}

			//Free resources
			imp.FreeScene();
		}
	}
}