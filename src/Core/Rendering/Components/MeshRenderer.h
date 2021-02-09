#pragma once
#include "Renderer.h"
#include <Data/Mesh.h>
#include <Misc/Property.h>
#include <Core/TypeRegister.h>

namespace Tristeon::Core::Rendering
{
	/**
	 * \brief The MeshRenderer component renders 3D meshes
	 */
	class MeshRenderer : public Renderer
	{
	public:
		/**
		* \brief The Mesh of the meshrenderer
		*/
		Property(MeshRenderer, mesh, Data::SubMesh);
		SetProperty(mesh)
		{
			_mesh = value;
			//TODO: Update mesh
		}
		GetProperty(mesh) { return _mesh; }

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;

		void render() override;
	private:
		Data::SubMesh _mesh;
		std::string meshFilePath;
		uint32_t subMeshID = 0;

		REGISTER_TYPE_H(MeshRenderer)
	};
}