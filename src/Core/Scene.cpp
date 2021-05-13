#include <Core/Scene.h>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include <Core/RenderData.h>
#include <Core/Rendering/RenderManager.h>

#include "Collector.h"
#include "Rendering/Components/Renderer.h"
#include <Core/Rendering/Material.h>

#include "Components/Camera.h"
#include "Data/Resources.h"

namespace Tristeon::Core
{
	REGISTER_TYPE_CPP(Scene)

	nlohmann::json Scene::serialize()
	{
		nlohmann::json output;
		output["typeID"] = Type<Scene>::fullName();
		output["name"] = name;
		nlohmann::json jsonArray = nlohmann::json::array();
		for (int i = 0; i < gameObjects.size(); i++)
		{
			jsonArray.push_back(gameObjects[i]->serialize());
		}
		output["gameObjects"] = jsonArray;
		return output;
	}

	void Scene::deserialize(nlohmann::json json)
	{
		nlohmann::json gameObjectData = json["gameObjects"];
		if (gameObjectData.is_array())
		{
			for (auto iterator = gameObjectData.begin(); iterator != gameObjectData.end(); ++iterator)
			{
				std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>();
				gameObject->deserialize(iterator->get<nlohmann::json>());
				gameObjects.push_back(std::move(gameObject));
			}
		}
		else
		{
			std::cout << "Deserialization of the scene is going goofy, ur probably deserializing the scene with a wrong json format";
		}
		const std::string nameValue = json["name"];
		name = nameValue;
	}

	void Scene::recordSceneCmd(vk::CommandBuffer cmd, const uint8_t& frameIndex, vk::Framebuffer framebuffer)
	{
		//Empty submit
		if (Collector<Components::Camera>::all().empty())
		{
			vk::CommandBufferBeginInfo begin{ {}, nullptr };
			VULKAN_DEBUG(cmd.begin(&begin));
			{
				std::array<vk::ClearValue, 3> clear{
					vk::ClearColorValue{std::array<float, 4>{0, 0, 0, 0} },
					vk::ClearDepthStencilValue{ 0, 0 },
					vk::ClearColorValue{std::array<float, 4>{0, 0, 0, 0} }
				};

				const vk::RenderPassBeginInfo pass_begin{
					renderData.offscreenPass,
					framebuffer,
					vk::Rect2D { vk::Offset2D { 0, 0 }, renderData.extent },
					(uint32_t)clear.size(), clear.data()
				};

				cmd.beginRenderPass(pass_begin, vk::SubpassContents::eInline);
				cmd.endRenderPass();

			}
			cmd.end();
			return;
		}

		auto* cam = Collector<Components::Camera>::all()[0];
		auto projection = cam->getProjectionMatrix((float)renderData.extent.width / (float)renderData.extent.height);
		projection[1][1] *= -1;
		const auto view = cam->getViewMatrix();
		
		vk::CommandBufferBeginInfo begin{ {}, nullptr };
		VULKAN_DEBUG(cmd.begin(&begin));
		{
			std::array<vk::ClearValue, 3> clear{
				vk::ClearColorValue{std::array<float, 4>{0, 0, 0, 0} },
				vk::ClearDepthStencilValue{ 0, 0 },
				vk::ClearColorValue{std::array<float, 4>{0, 0, 0, 0} }
			};

			const vk::RenderPassBeginInfo pass_begin{
				renderData.offscreenPass,
				framebuffer,
				vk::Rect2D { vk::Offset2D { 0, 0 }, renderData.extent },
				(uint32_t)clear.size(), clear.data()
			};

			cmd.beginRenderPass(pass_begin, vk::SubpassContents::eInline);
			{
				auto renderers = Collector<Rendering::Renderer>::all();
				for (auto* renderer : renderers)
				{
					if (renderer->material.get() == nullptr)
						continue;

					auto* material = renderer->material.get();
					cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline());
					renderer->render(cmd, frameIndex, projection, view);
				}
			}
			cmd.endRenderPass();
		}
		cmd.end();
	}

	Scene::~Scene()
	{
		Data::Resources::clearSceneCache();
	}

	void Scene::add(std::unique_ptr<GameObject> gameObj)
	{
		gameObj->deserialize(gameObj->serialize());
		gameObjects.push_back(std::move(gameObj));
	}

	void Scene::remove(GameObject* gameObj)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (gameObjects[i].get() == gameObj) { gameObjects.erase(gameObjects.begin() + i); }
		}
	}

	GameObject* Scene::get(std::string instanceID)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (gameObjects[i]->getInstanceID() == instanceID) return gameObjects[i].get();
		}
		std::cout << "Couldn't find gameObject\n";
		return nullptr;
	}
}
