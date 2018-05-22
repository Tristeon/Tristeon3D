#pragma once
#include "Renderer.h"
#include "Data/Mesh.h"
#include "Misc/Property.h"
#include "Editor/TypeRegister.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			/**
			 * \brief The MeshRenderer component renders meshes
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
					if (renderer != nullptr)
						renderer->onMeshChange(value);
				}
				GetProperty(mesh) { return _mesh; }
				
				/**
				* \brief Creates and initializes the internal renderer
				* \param data Rendering binding data, used to initialize the renderer
				*/
				void initInternalRenderer(BindingData* data) override;

				nlohmann::json serialize() override;
				void deserialize(nlohmann::json json) override;
			private:
				/**
				 * \brief The mesh of the meshrenderer
				 */
				Data::SubMesh _mesh;

				std::string meshFilePath = "";
				uint32_t subMeshID = 0;

				REGISTER_TYPE_H(MeshRenderer)
			};
		}
	}
}
