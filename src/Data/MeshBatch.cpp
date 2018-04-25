#include "MeshBatch.h"
#include <valarray>

namespace Tristeon
{
	namespace Data
	{
		std::map<std::string, std::unique_ptr<Mesh>> MeshBatch::loadedMeshes;

		SubMesh MeshBatch::getSubMesh(std::string meshPath, uint32_t subMeshID)
		{
			//Can't find it? load it in 
			if (loadedMeshes.find(meshPath) == end(loadedMeshes))
				loadMesh(meshPath);

			//Get our loaded mesh
			Mesh* m = loadedMeshes[meshPath].get();
			//Can't find the correct submesh
			if (subMeshID > m->submeshes.size() - 1|| m->submeshes.size() == 0)
				return SubMesh();

			//Found the correct submesh
			return loadedMeshes[meshPath]->submeshes[subMeshID];
		}

		Mesh* MeshBatch::loadMesh(std::string meshPath)
		{
			//Loads in the mesh at the given filepath
			std::unique_ptr<Mesh> m = std::make_unique<Mesh>();
			m->load(meshPath);
			loadedMeshes[meshPath] = move(m);
			return m.get();
		}

		void MeshBatch::unloadMesh(std::string meshPath)
		{
			//Don't have it, return
			if (loadedMeshes.find(meshPath) == loadedMeshes.end())
				return;

			//Remove, unique ptr will take care of destruction
			loadedMeshes.erase(meshPath);
		}

		void MeshBatch::unloadAll()
		{
			//Clear all, unique ptr will take care of destruction
			loadedMeshes.empty();
		}
	}
}
