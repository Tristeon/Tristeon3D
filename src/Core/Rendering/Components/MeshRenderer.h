#pragma once
#include "Renderer.h"
#include <Data/Mesh.h>
#include <Misc/Property.h>

#include <Core/TypeRegister.h>
#include <Core/Rendering/Helper/Buffer.h>

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
			createBuffers();
		}
		GetProperty(mesh) { return _mesh; }

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;

		void render() override;
	private:
		void createBuffers();
		
		Data::SubMesh _mesh;
		std::string meshFilePath;
		uint32_t subMeshID = 0;

		std::unique_ptr<Buffer> vertices;
		std::unique_ptr<Buffer> indices;

		REGISTER_TYPE_H(MeshRenderer)
	};
}
