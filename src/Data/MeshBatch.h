#pragma once
#include "Mesh.h"

namespace Tristeon
{
	namespace Scenes { class SceneManager; }

	namespace Data
	{
		/**
		 * The mesh batch loads mesh files from disc into memory. 
		 * It stores the meshes until it's told to clean up.
		 */
		class MeshBatch
		{
		public:
			friend Scenes::SceneManager;

			/**
			 * Gets a submesh based on the filepath and the index of the submesh
			 * \param meshPath The filepath of the mesh file
			 * \param subMeshIndex The index of the submesh inside of the mesh file
			 */
			static SubMesh getSubMesh(std::string meshPath, uint32_t subMeshIndex);

			/**
			 * Loads in a mesh with the given filepath
			 * \param meshPath The filepath of the mesh
			 * \return Returns the loaded in mesh
			 */
			static Mesh* loadMesh(std::string meshPath);
			/**
			 * Unloads a mesh with the given filepath
			 * \param meshPath The path of the mesh
			 */
			static void unloadMesh(std::string meshPath);

			static void unloadAll();
		private:
			static std::map<std::string, std::unique_ptr<Mesh>> loadedMeshes;
		};
	}
}
