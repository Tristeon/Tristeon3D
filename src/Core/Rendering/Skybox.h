#pragma once
#include "Core/TObject.h"
#include <Editor/TypeRegister.h>
#include <glm/mat4x4.hpp>
#include "Data/Mesh.h"

#if TRISTEON_EDITOR
namespace Tristeon {
	namespace Editor {
		class SkyboxFileItem;
	}
}
#endif


namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			class Skybox : public TObject
			{
#if TRISTEON_EDITOR
				friend Editor::SkyboxFileItem;
#endif
			public:
				Skybox();
				virtual ~Skybox() = default;
				nlohmann::json serialize() override;
				void deserialize(nlohmann::json json) override;

			protected:
				virtual void init() { };
				virtual void draw(glm::mat4 view, glm::mat4 proj) { };

				std::string texturePath;
				bool isDirty = true;

				Data::SubMesh mesh;
				bool cubemapLoaded = false;
			private:
				REGISTER_TYPE_H(Skybox)
			};
		}
	}
}
