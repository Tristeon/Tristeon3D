#include "Skybox.h"
#include <fstream>
#include <filesystem>

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			REGISTER_TYPE_CPP(Skybox)

			nlohmann::json Skybox::serialize()
			{
				nlohmann::json j;
				j["typeID"] = typeid(Skybox).name();
				j["left"] = texNames.left;
				j["right"] = texNames.right;
				j["up"] = texNames.up;
				j["down"] = texNames.down;
				j["forward"] = texNames.forward;
				j["back"] = texNames.back;
				return j;
			}

			void Skybox::updateTextures()
			{
				using namespace std::experimental::filesystem;

				if (exists(path(texNames.left)))
					texImages.left = Data::Image(texNames.left);
				if (exists(path(texNames.right)))
					texImages.right = Data::Image(texNames.right);
				if (exists(path(texNames.up)))
					texImages.up = Data::Image(texNames.up);
				if (exists(path(texNames.down)))
					texImages.down = Data::Image(texNames.down);
				if (exists(path(texNames.forward)))
					texImages.forward = Data::Image(texNames.forward);
				if (exists(path(texNames.back)))
					texImages.back = Data::Image(texNames.back);

				isDirty = true;
			}

			void Skybox::deserializeTexture(nlohmann::json j, std::string name, std::string& oldVal, Data::Image& img)
			{
				std::string val = j[name];
				if (val != oldVal)
				{
					img = Data::Image(val);
					isDirty = true;
				}
				oldVal = val;
			}

			size_t Skybox::getSize()
			{
				return texImages.left.getSize() +
					texImages.right.getSize() +
					texImages.up.getSize() +
					texImages.down.getSize() +
					texImages.forward.getSize() +
					texImages.back.getSize();
			}

			float Skybox::getVertexCount() const
			{
				return vertexCount;
			}

			float* Skybox::getVertices()
			{
				if (vertices == nullptr)
				{
					vertices = new float[]{
						// positions          
						-1.0f,  1.0f, -1.0f,
						-1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f,  1.0f, -1.0f,
						-1.0f,  1.0f, -1.0f,

						-1.0f, -1.0f,  1.0f,
						-1.0f, -1.0f, -1.0f,
						-1.0f,  1.0f, -1.0f,
						-1.0f,  1.0f, -1.0f,
						-1.0f,  1.0f,  1.0f,
						-1.0f, -1.0f,  1.0f,

						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f,  1.0f,
						1.0f,  1.0f,  1.0f,
						1.0f,  1.0f,  1.0f,
						1.0f,  1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,

						-1.0f, -1.0f,  1.0f,
						-1.0f,  1.0f,  1.0f,
						1.0f,  1.0f,  1.0f,
						1.0f,  1.0f,  1.0f,
						1.0f, -1.0f,  1.0f,
						-1.0f, -1.0f,  1.0f,

						-1.0f,  1.0f, -1.0f,
						1.0f,  1.0f, -1.0f,
						1.0f,  1.0f,  1.0f,
						1.0f,  1.0f,  1.0f,
						-1.0f,  1.0f,  1.0f,
						-1.0f,  1.0f, -1.0f,

						-1.0f, -1.0f, -1.0f,
						-1.0f, -1.0f,  1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						-1.0f, -1.0f,  1.0f,
						1.0f, -1.0f,  1.0f
					};
				}
				return vertices;
			}

			void Skybox::deserialize(nlohmann::json json)
			{
				deserializeTexture(json, "left", texNames.left, texImages.left);
				deserializeTexture(json, "right", texNames.right, texImages.right);
				deserializeTexture(json, "up", texNames.up, texImages.up);
				deserializeTexture(json, "down", texNames.down, texImages.down);
				deserializeTexture(json, "forward", texNames.forward, texImages.forward);
				deserializeTexture(json, "back", texNames.back, texImages.back);
			}
		}
	}
}
