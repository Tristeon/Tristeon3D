#include "MeshBatch.h"
#include <valarray>

namespace Tristeon
{
	namespace Data
	{
		std::map<std::string, std::unique_ptr<Mesh>> MeshBatch::loadedMeshes;

		SubMesh MeshBatch::getSubMesh(std::string meshPath, uint32_t subMeshID)
		{
			if (loadedMeshes.find(meshPath) == end(loadedMeshes))
				loadMesh(meshPath);

			Mesh* m = loadedMeshes[meshPath].get();
			if (subMeshID > m->submeshes.size() || m->submeshes.size() == 0)
			{
				return SubMesh();
			}

			return loadedMeshes[meshPath]->submeshes[subMeshID];
		}

		Mesh* MeshBatch::loadMesh(std::string meshPath)
		{
			std::unique_ptr<Mesh> m = std::make_unique<Mesh>();
			m->load(meshPath);
			loadedMeshes[meshPath] = move(m);
			return m.get();
		}

		void MeshBatch::unloadMesh(std::string meshPath)
		{
			if (loadedMeshes.find(meshPath) != loadedMeshes.end())
				return;
			loadedMeshes.erase(meshPath);
		}

		void MeshBatch::unloadAll()
		{
			loadedMeshes.empty();
		}
	}
}
