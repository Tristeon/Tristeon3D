#include "Renderer.h"
#include "Core/Collector.h"
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
			material = Resources::jsonLoad<Material>(pathValue);
		materialPath = pathValue;
	}

	Renderer::Renderer()
	{
		Collector<Renderer>::add(this);
	}

	Renderer::~Renderer()
	{
		Collector<Renderer>::remove(this);
	}
}
