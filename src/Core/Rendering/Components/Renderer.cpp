#include "Renderer.h"
#include "Core/Collector.h"
#include "Data/Resources.h"

namespace Tristeon::Core::Rendering
{
	void Renderer::render()
	{
	}

	nlohmann::json Renderer::serialize()
	{
		nlohmann::json j = Component::serialize();
		j["material"] = materialPath;
		return j;
	}

	void Renderer::deserialize(nlohmann::json json)
	{
		Component::deserialize(json);
		
		const auto pathValue = json.value("material", std::string());
		if (pathValue != materialPath)
			material = Resources::jsonLoad<Material>(materialPath);
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
