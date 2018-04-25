#pragma once
#include "Mesh.h"

namespace Tristeon
{
	namespace Scenes { class SceneManager; }

	namespace Data
	{
		/**
		 * \brief The mesh batch is used to load in mesh files and store them until they're needed.
		 * The mesh batch defines functionality for loading, unloading and getting mesh data
		 */
		class MeshBatch
		{
		public:
			friend Scenes::SceneManager;

			/**
			 * \brief Returns a submesh based on the filepath and the id of the submesh
			 * \param meshPath The filepath of the mesh file
			 * \param subMeshID The id of the submesh inside of the mesh file
			 * \return Returns the requested submesh
			 */
			static SubMesh getSubMesh(std::string meshPath, uint32_t subMeshID);

			/**
			 * \brief Loads in a mesh with the given filepath
			 * \param meshPath The filepath of the mesh
			 * \return Returns the loaded in mesh
			 */
			static Mesh* loadMesh(std::string meshPath);
			/**
			 * \brief Unloads a mesh with the given filepath
			 * \param meshPath The path of the mesh
			 */
			static void unloadMesh(std::string meshPath);

		private:
			/**
			 * \brief All the loaded in meshes
			 */
			static std::map<std::string, std::unique_ptr<Mesh>> loadedMeshes;
			/**
			 * \brief Unloads all of the loaded meshes
			 */
			static void unloadAll();
		};
	}
}
