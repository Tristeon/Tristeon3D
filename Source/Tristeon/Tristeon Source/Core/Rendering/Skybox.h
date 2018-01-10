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

			protected:
				float getVertexCount() const;
				float* getVertices();

				virtual void init() { };
				virtual void draw() { };

				void updateTextures();
				void deserializeTexture(nlohmann::json j, std::string name, std::string& oldVal, Data::Image& img);
				struct 
				{
					std::string left = "", right = "", up = "", down = "", forward = "", back = "";

					std::string operator[] (int i) const
					{
						std::string arr[] = { left, right, up, down, forward, back };
						return arr[i];
					}
				} texNames;

				struct
				{
					Data::Image left, right, up, down, forward, back;
					
					Data::Image operator[] (int i) const
					{
						Data::Image arr[] = { left, right, up, down, forward, back };
						return arr[i];
					}
				} texImages;

				bool isDirty = true;

				float* vertices = nullptr;
				const float vertexCount = 108;

			private:
				REGISTER_TYPE_H(Skybox)
			};
		}
	}
}
