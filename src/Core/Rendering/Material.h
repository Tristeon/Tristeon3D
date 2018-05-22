#pragma once
#include "Core/TObject.h"
#include "Data/Image.h"
#include "Misc/Color.h"
#include <glm/mat4x4.hpp>
#include "Editor/TypeRegister.h"
#include "ShaderFile.h"
#include "Math/Vector3.h"

#ifdef TRISTEON_EDITOR
namespace Tristeon {
	namespace Editor {
		class MaterialFileItem;
	}
}
#endif

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
#ifdef TRISTEON_EDITOR
				friend Editor::MaterialFileItem;
#endif
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

				/**
				 * \brief Sets the texture property with the name [name] to the given Image
				 * \param name The name of the texture property. The function will return a warning if this name hasn't been registered.
				 * \param path The path to the new texture
				 */
				virtual void setTexture(std::string name, std::string path);
				/**
				 * \brief Sets the float property with the name [name] to the given value
				 * \param name The name of the float property. The function will return a warning if this name hasn't been registered.
				 * \param value The new value of the float property
				 */
				virtual void setFloat(std::string name, float value);
				/**
				 * \brief Sets the vector3 property with the name [name] to the given vector
				 * \param name The name of the vector3 property. The function will return a warning if this name hasn't been registered.
				 * \param value The new value of the vector3 property
				 */
				virtual void setVector3(std::string name, Math::Vector3 value);
				/**
				 * \brief Sets the color property with the name [name] to the given color
				 * \param name The name of the color property. The function will return a warning if this name hasn't been registered.
				 * \param value The new value of the vector3 property
				 */
				virtual void setColor(std::string name, Misc::Color value);
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
				virtual void render(glm::mat4 model, glm::mat4 view, glm::mat4 proj) { }

				/**
				 * \brief Setup textures initializes all the textures and uploads them to the GPU. API specific.
				 */
				virtual void setupTextures() { }

				/**
				 * \brief Used to rebuild the shader pipeline and update our properties when we've changed ShaderFile
				 */
				virtual void updateProperties(bool updateResources = true) { }

				virtual void updateShader();

				/**
				* \brief The shaderfile, loaded using shaderFilePath
				*/
				std::unique_ptr<ShaderFile> shader;

				/**
				 * \brief The texturepaths, used to load images for the material.
				 */
				std::map<std::string, std::string> texturePaths;
				/**
				 * \brief Vector properties, used to send vector data to the GPU
				 */
				std::map<std::string, Math::Vector3> vectors;
				/**
				 * \brief Color properties, used to send color data to the gpu
				 */
				std::map<std::string, Misc::Color> colors;
				/**
				 * \brief Float properties, used to send float data to the gpu
				 */
				std::map<std::string, float> floats;
			private:
				void checkProperty(std::string parentName, ShaderProperty prop, const std::map<std::string, std::string>& tex, const std::map<std::string, Math::Vector3>& vec, const std::map<std::string, Misc::Color>& col, const std::map<std::string, float>& fl);
				/**
				 * \brief The filepath to the shader file
				 */
				std::string shaderFilePath;

				REGISTER_TYPE_H(Material)
			};
		}
	}
}
