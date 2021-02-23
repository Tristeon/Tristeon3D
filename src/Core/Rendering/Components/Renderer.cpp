#include "Renderer.h"

#include "Core/BindingData.h"
#include "Core/Collector.h"
#include "Core/Transform.h"
#include "Data/Resources.h"

namespace Tristeon::Core::Rendering
{
	nlohmann::json Renderer::serialize()
	{
		nlohmann::json j = Component::serialize();
		j["typeID"] = Type<Renderer>::fullName();
		j["materialPath"] = materialPath;
		return j;
	}

	void Renderer::deserialize(nlohmann::json json)
	{
		Component::deserialize(json);
		
		const auto pathValue = json.value("materialPath", std::string());
		if (pathValue != materialPath || !material)
			material = Data::Resources::jsonLoad<Material>(pathValue, Data::Resources::CacheBehaviour::SceneBased);
		materialPath = pathValue;
	}

	void Renderer::createDescriptorSet()
	{
		//TODO: Static objects could (partially) be created with staged buffers
		transformBuffer = std::make_unique<Buffer>(sizeof(Transform::UniformBuffer), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		
		transformSet = binding_data.device.allocateDescriptorSets({ binding_data.descriptorPool, binding_data.transformLayout })[0];

		auto buffer = vk::DescriptorBufferInfo(transformBuffer->buffer, 0, sizeof(Transform::UniformBuffer));
		vk::WriteDescriptorSet write{ transformSet, 0, 0, vk::DescriptorType::eUniformBuffer, {}, buffer, {}};
		binding_data.device.updateDescriptorSets(write, {});
	}

	Renderer::Renderer()
	{
		Collector<Renderer>::add(this);

		createDescriptorSet();
	}

	Renderer::~Renderer()
	{
		Collector<Renderer>::remove(this);

		binding_data.device.freeDescriptorSets(binding_data.descriptorPool, transformSet);
	}
}