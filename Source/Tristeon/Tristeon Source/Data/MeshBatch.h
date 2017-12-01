#pragma once
#include "Mesh.h"

namespace Tristeon
{
	namespace Scenes { class SceneManager; }

	namespace Data
	{
		class MeshBatch
		{
		public:
			friend Scenes::SceneManager;

			static SubMesh getSubMesh(std::string meshPath, uint32_t subMeshID);

			static Mesh* loadMesh(std::string meshPath);
			static void unloadMesh(std::string meshPath);

		private:
			static std::map<std::string, std::unique_ptr<Mesh>> loadedMeshes;
			static void unloadAll();
		};
	}
}
