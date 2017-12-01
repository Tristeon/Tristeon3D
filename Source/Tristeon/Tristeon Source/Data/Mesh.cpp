#include "Mesh.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Misc/Console.h"
#include "Core/Settings.h"

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

			switch(Core::Settings::getRenderAPI())
			{
			case Core::Rendering::RAPI_Vulkan: 
				//Vulkan has flipped uvs
				postProcess |= aiProcess_FlipUVs;
				break;
			default: 
				break;
			}

			//Read the file
			const aiScene* scene = imp.ReadFile(
					filePath, postProcess);
			if (!scene)
				return;

			//Go over the meshes
			if (scene->HasMeshes())
			{
				for (int i = 0; i < scene->mNumMeshes; i++)
				{
					//Create submesh
					SubMesh submesh;
					aiMesh* aiMesh = scene->mMeshes[i];
					//Load vertices
					for (int j = 0; j < aiMesh->mNumVertices; j++)
					{
						//Positions, normals, uvs
						aiVector3D const p = aiMesh->mVertices[j];
						aiVector3D const n = aiMesh->mNormals[j];
						aiVector3D const t = aiMesh->mTextureCoords[0] ? aiMesh->mTextureCoords[0][j] : aiVector3D(0, 0, 0);

						//Create vertex and add to our list
						Vertex v;
						v.pos = glm::vec3(p.x, p.y, p.z);
						v.normal = glm::vec3(n.x, n.y, n.z);
						v.texCoord = glm::vec2(float(t.x), float(t.y));
						submesh.vertices.push_back(v);
					}

					//Faces
					for (int j = 0; j < aiMesh->mNumFaces; j++)
					{
						//Get indices
						const aiFace f = aiMesh->mFaces[j];
						Misc::Console::t_assert(f.mNumIndices == 3, "This is not a triangle!");
						submesh.indices.push_back(f.mIndices[0]);
						submesh.indices.push_back(f.mIndices[1]);
						submesh.indices.push_back(f.mIndices[2]);
					}

					//Set material (temporary)
					submesh.materialID = aiMesh->mMaterialIndex;
					submeshes.push_back(submesh);
				}
			}

			//Free resources
			imp.FreeScene();
		}
	}
}