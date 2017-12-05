#pragma once
#include "Core/TObject.h"
#include "Data/Image.h"
#include "Misc/Color.h"
#include <glm/mat4x4.hpp>
#include "Editor/TypeRegister.h"
#include <filesystem>
#include <Misc/Property.h>
#include "ShaderFile.h"
#include "Editor/JsonSerializer.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		namespace Components { class Camera; }

		namespace Rendering
		{
			//Forward decl
			namespace Vulkan { class RenderManager; }
			class RenderManager;

			/**
			 * \brief The material controls the appearance of renderers in the world
			 */
			class Material : public TObject
			{
				friend Vulkan::RenderManager;
				friend RenderManager;

			public:
				/**
				 * \brief Serializes the material into a json file
				 * \return Returns the json object describing the material's information
				 */
				nlohmann::json serialize() override;
				/**
				 * \brief Deserializes the material from a json file
				 * \param json Returns the json object describing the material's information
				 */
				void deserialize(nlohmann::json json) override;

				Misc::Color getDiffuse() { return diffuseColor; }

				/**
				* \brief The diffuse image of the material
				*/
				Property(Data::Image, diffuse);
				GetPropertyConst(diffuse) { return _diffuse; }
				virtual SetProperty(diffuse) { _diffuse = value; }
			protected:
				/**
				 * \brief Initializes the material. Can be overriden by API specific behavior
				 */
				virtual void init() {}
				/**
				 * \brief OnRender function, override this function to send data to the shader. API specific.
				 * \param model The transformation matrix of the object
				 * \param view The current camera view matrix
				 * \param proj The current camera projection matrix
				 */
				virtual void render(glm::mat4 model, glm::mat4 view, glm::mat4 proj){};
				/**
				 * \brief Setup textures initializes all the textures and uploads them to the GPU. API specific.
				 */
				virtual void setupTextures(){};

				/**
				 * \brief Function callback used to rebuild the shader pipeline when we've changed ShaderFile
				 */
				virtual void rebuildShader(){};

				/**
				 * \brief The diffuse texture of the object
				 */
				Data::Image _diffuse;
				/**
				 * \brief The diffuse color of the object
				 */
				Misc::Color diffuseColor;

				/**
				 * \brief Keeps track of wether the material is done setting up (important for image loading / unloading on image change)
				 */
				bool prepared = false;
				/**
				* \brief The shaderfile, loaded using shaderFilePath
				*/
				std::unique_ptr<ShaderFile> shader;
			private:
				
				/**
				 * \brief The path to the diffuse image
				 */
				std::string diffusePath;

				/**
				 * \brief The filepath to the shader file
				 */
				std::string shaderFilePath;

				REGISTER_TYPE_H(Material)
			};

			inline nlohmann::json Material::serialize()
			{
				nlohmann::json j;
				j["typeID"] = typeid(Material).name();
				j["diffusePath"] = diffusePath;
				j["diffuseColor"] = diffuseColor.serialize();
				j["shaderFilePath"] = shaderFilePath;
				return j;
			}

			inline void Material::deserialize(nlohmann::json json)
			{
				const std::string diffusePathValue = json["diffusePath"];
				if (std::experimental::filesystem::exists(diffusePath))
					diffuse = Data::Image(diffusePath);
				diffusePath = diffusePathValue;

				diffuseColor.deserialize(json["diffuseColor"]);

				const std::string shaderFilePathValue = json["shaderFilePath"];
				if (shaderFilePath != shaderFilePathValue)
				{
					shader = std::unique_ptr<ShaderFile>(JsonSerializer::deserialize<ShaderFile>(shaderFilePath));
					rebuildShader();
				}
				shaderFilePath = shaderFilePathValue;
			}
		}
	}
}
