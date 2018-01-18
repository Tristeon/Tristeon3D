#pragma once
#include "Core/Managers/Manager.h"
#include "Misc/Delegate.h"
#include "Misc/vector.h"
#include "Skybox.h"

namespace Tristeon
{
	namespace Core
	{
		//Forward decl
		class BindingData;
		class Message;
		namespace Components { class Camera; }

		namespace Rendering
		{
			//Forward decl
			class Material;
			class RenderTechnique;
			class Renderer;
			class ShaderFile;

			/**
			 * \brief UIRenderable is a struct that's used to render UI after the scene has been rendered.
			 */
			struct UIRenderable : TObject
			{
				/**
				 * \brief UIRenderable.data is used by API specific renderers to store rendering information that should be sent to the user
				 */
				TObject* data;
				/**
				 * \brief Subscribe to this function to implement your render function.
				 */
				Misc::Delegate<> onRender;
			};

			/**
			 * \brief RenderManager is the base class of RenderManagers and gets overriden to define API specific behavior.
			 * This class defines standard behavior for (de)registering (ui)renderers, and it manages materials and shaders.
			 */
			class RenderManager : public Managers::Manager
			{
			public:
				/**
				 * \brief Creates a new RenderManager
				 * \param data BindingData gets stored as reference to share render information with other engine systems
				 */
				RenderManager(BindingData* data);
				/**
				 * \brief Sets up all the callbacks and gets shader information
				 */
				void init() override;

				/**
				 * \brief Render is an abstract function that is intended to be defined by API specific subclasses. It gets called in the window MT_RENDER callback.
				 */
				virtual void render() = 0;
			
				/**
				 * \brief Gets all the registered renderers and returns them as a vector/list
				 * \return Returns the renderers as a std::vector
				 */
				std::vector<Renderer*> getRenderers() const;

				/**
				 * \brief Enables/Disables the grid based on the given parameter. The grid is automatically disabled in play mode.
				 * \param enable Defines wether or not the grid should be enabled (true/false)
				 */
				virtual void setGridEnabled(bool enable);

				/**
				* \brief Returns a material serialized from the given filepath
				* \param filePath The filepath of the material
				* \return A material serialized from the given filepath, or from the cached materials
				*/
				static Material* getMaterial(std::string filePath);
				
				static Skybox* getSkybox(std::string filePath);
			protected:
				virtual Skybox* _getSkybox(std::string filePath) = 0;

				/**
				* \brief Returns a material serialized from the given filepath
				* \param filePath The filepath of the material
				* \return A material serialized from the given filepath, or from the cached materials
				*/
				virtual Material* getmaterial(std::string filePath) = 0;

				/**
				 * \brief Removes all the registered renderers. Is generally called when a scene is closed to make sure the list is clean.
				 */
				void reset() override;
				/**
				 * \brief Registers a renderer to the renderers or UIrenderers list. 
				 * \param msg The message coming from the manager protocol message system. Message.userData is expected to contain our renderer
				 */
				virtual TObject* registerRenderer(Message msg);
				/**
				 * \brief Deregisters a renderer from the renderers or UIRenderers list.
				 * \param msg The message coming from the manager protocol message system. Message.userData is expected to contain our renderer
				 */
				virtual TObject* deregisterRenderer(Message msg);

				/**
				 * \brief Registers a camera to the camera list.
				 * \param msg The message coming from the manager protocol message system. Message.userData is expected to contain our camera
				 * \return Returns the newly aded camera. the result of this function is used by inherited classes to know what camera has been added.
				 */
				virtual Components::Camera* registerCamera(Message msg);
				/**
				* \brief Deregisters a camera from the camera list.
				* \param msg The message coming from the manager protocol message system. Message.userData is expected to contain our camera
				* \return Returns the removed camera. the result of this function is used by inherited classes to know what camera has been added.
				*/
				virtual Components::Camera* deregisterCamera(Message msg);

				/**
				 * \brief The renderTechnique renders the cameras and the scene.
				 */
				RenderTechnique* technique = nullptr;

				/**
				 * \brief The cameras in the current active scene
				 */
				vector<Components::Camera*> cameras;

				/**
				 * \brief The renderers int he current active scene
				 */
				Tristeon::vector<Renderer*> renderers;
				/**
				 * \brief All the UIrenderables 
				 */
				Tristeon::vector<UIRenderable*> renderables;
				/**
				 * \brief All the materials in the project, sorted by their ID
				 */
				std::map<std::string, Material*> materials;

				std::map < std::string, std::unique_ptr<Skybox>> skyboxes;

				/**
				 * \brief All the shaderfiles in the project
				 */
				std::vector<ShaderFile> shaderFiles;

				/**
				 * \brief A reference to bindingData, used to share binding data between engine systems
				 */
				BindingData* bindingData;

				/**
				 * \brief Keeps track of wether the engine is in playmode or not.
				 */
				bool inPlayMode = false;
				/**
				 * \brief Keeps track of wether the grid should be rendered or not.
				 */
				bool gridEnabled = true;

				/**
				 * \brief The only instance of RenderManager ever. Used so that getMaterial() can access local variables
				 */
				static RenderManager* instance;
			};
		}
	}
}
