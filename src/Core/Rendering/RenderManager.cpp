#include "RenderManager.h"
#include "Core/ManagerProtocol.h"
#include "Core/GameObject.h"
#include "Core/Message.h"
#include <Core/Components/Camera.h>
#include <Core/Rendering/Components/Renderer.h>
#include <Misc/Console.h>

#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			RenderManager* RenderManager::instance;

			RenderManager::RenderManager(BindingData* data) : bindingData(data)
			{
				//Store instance
				instance = this;
			}

			void RenderManager::init()
			{
				Manager::init();

				//Render
				ManagerProtocol::subscribeToMessage(MT_RENDER, [&](Message msg) { render(); });

				//(De)registering of render components
				ManagerProtocol::subscribeToMessage(MT_RENDERINGCOMPONENT_REGISTER, [&](Message msg) { registerRenderer(msg); });
				ManagerProtocol::subscribeToMessage(MT_RENDERINGCOMPONENT_DEREGISTER, [&](Message msg) { deregisterRenderer(msg); });

				//(De)registering of cameras
				ManagerProtocol::subscribeToMessage(MT_CAMERA_REGISTER, [&](Message msg) { registerCamera(msg); });
				ManagerProtocol::subscribeToMessage(MT_CAMERA_DEREGISTER, [&](Message msg) { deregisterCamera(msg); });

				//Game logic
				ManagerProtocol::subscribeToMessage(MT_GAME_LOGIC_START, [&](Message msg) { inPlayMode = true; });
				ManagerProtocol::subscribeToMessage(MT_GAME_LOGIC_STOP, [&](Message msg) { inPlayMode = false; });
			}

			std::vector<Renderer*> RenderManager::getRenderers() const
			{
				return renderers;
			}

			TObject* RenderManager::registerRenderer(Message msg)
			{
				//Confirm that we're getting useful data
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register null renderer!");

				//Check if renderer
				Renderer* r = dynamic_cast<Renderer*>(msg.userData);
				if (r != nullptr)
				{
					//Successfully found a renderer
					renderers.push_back(r);
					//Init
					r->initInternalRenderer(bindingData);
					return r;
				}
				else
				{
					//Try to get a UI renderable instead
					UIRenderable* rable = dynamic_cast<UIRenderable*>(msg.userData);
					Misc::Console::t_assert(rable != nullptr, "Couldn't cast userdata to renderer or ui renderable in registerRenderer()!");
					renderables.push_back(rable);

					return rable;
				}
			}

			TObject* RenderManager::deregisterRenderer(Message msg)
			{
				//Confirm that we're getting useful data
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to deregister null renderer!");

				//Check if the given userdata is a renderer, if so, remove from our list
				Renderer* r = dynamic_cast<Renderer*>(msg.userData);
				if (r != nullptr)
					renderers.remove(r);
				else
				{
					//Check if the given userdata is a UIRenderable instead
					UIRenderable* rable = dynamic_cast<UIRenderable*>(msg.userData);
					Misc::Console::t_assert(rable != nullptr, "Couldn't cast userdata to renderer or ui renderable in deregisterRenderer()!");
					renderables.remove(rable);
				}

				return msg.userData;
			}

			Components::Camera* RenderManager::registerCamera(Message msg)
			{
				//Confirm that we're getting useful data
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to register null camera!");

				//Try to cast to camera, add to our list if successful
				Components::Camera* cam = dynamic_cast<Components::Camera*>(msg.userData);
				Misc::Console::t_assert(cam != nullptr, "Couldn't cast userdata to camera (registerCamera())!");
				cameras.push_back(cam);
				return cam;
			}

			Components::Camera* RenderManager::deregisterCamera(Message msg)
			{
				//Confirm that we're getting useful data
				Misc::Console::t_assert(msg.userData != nullptr, "Trying to deregister null camera!");

				//Try to cast to camera, remove from our list if successful
				Components::Camera* cam = dynamic_cast<Components::Camera*>(msg.userData);
				Misc::Console::t_assert(cam != nullptr, "Couldn't cast userdata to camera (deregisterCamera())!");
				cameras.remove(cam);
				return cam;
			}

			void RenderManager::setGridEnabled(bool enable)
			{
				this->gridEnabled = enable;
			}

			Material* RenderManager::getMaterial(std::string filePath)
			{
				return instance->getmaterial(filePath);
			}

			Skybox* RenderManager::getSkybox(std::string filePath)
			{
				//Try to return the material from our batched materials
				if (instance->skyboxes.find(filePath) != instance->skyboxes.end())
					return instance->skyboxes[filePath].get(); //We keep ownership, give the user a reference

				//Don't even bother doing anything if the material doesn't exist
				if (!filesystem::exists(filePath))
					return nullptr;

				//Our materials can only be .mat files
				if (filesystem::path(filePath).extension() != ".skybox")
					return nullptr;

				return instance->_getSkybox(filePath);
			}
		}
	}
}
