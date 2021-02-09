#pragma once
#include <Core/Components/Component.h>
#include <Core/Rendering/Material.h>

namespace Tristeon::Core
{
	class SceneManager;
	
	namespace Rendering
	{
		/**
		 * \brief Renderer is the base class for all renderers in the engine. 
		 * It provides base functionality and wraps around a so called "InternalRenderer", 
		 * which is designed to be created based on the current selected rendering API.
		 */
		class Renderer : public Components::Component
		{
			friend RenderManager;
			friend SceneManager;
		public:
			Renderer();
			virtual ~Renderer() override;
			
			/**
			 * \brief Render is the main rendering function of the renderer and must be overridden by subtypes.
			 */
			virtual void render() = 0;

			/**
			* \brief The (shared) material of the renderer
			*/
			Property(Renderer, material, Material*);
			GetProperty(material) { return _material; }
			SetProperty(material) { _material = value; }

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;
		protected:
			/**
			 * \brief The Filepath of the material
			 */
			std::string materialPath;
			/**
			 * \brief The material
			 */
			Material* _material = nullptr;

			bool registered = false;
		};
	}
}
