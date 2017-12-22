#pragma once
#include "Core/TObject.h"
#include <Editor/TypeRegister.h>
#include "Data/Image.h"

#ifdef EDITOR
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
#ifdef EDITOR
				friend Editor::SkyboxFileItem;
#endif
			public:
				nlohmann::json serialize() override;
				void deserialize(nlohmann::json json) override;
				size_t getSize();

			private:
				REGISTER_TYPE_H(Skybox)

				void updateTextures();
				void deserializeTexture(nlohmann::json j, std::string name, std::string& oldVal, Data::Image& img);
				struct 
				{
					std::string left = "", right = "", up = "", down = "", forward = "", back = "";
				} texNames;

				struct
				{
					Data::Image left, right, up, down, forward, back;
				} texImages;

				bool isDirty = true;
			};
		}
	}
}
