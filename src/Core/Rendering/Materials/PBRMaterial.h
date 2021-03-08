#pragma once
#include <Core/Rendering/Material.h>
#include <Core/TypeRegister.h>

#include "Data/Image.h"

namespace Tristeon::Core::Rendering
{
	class PBRMaterial : public Material
	{
		REGISTER_TYPE_H(PBRMaterial);
	
	public:
		PBRMaterial() = default;
		virtual ~PBRMaterial() override;
		explicit PBRMaterial(PipelineProperties pProperties);

		ShaderFile* shader() override;
		void createPipeline() override;
		void createDescriptorSets() override;

	protected:
		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;

	private:
		Data::Image* albedo = nullptr;
		std::string albedoPath;
		Data::Image* normal = nullptr;
		std::string normalPath;
		Data::Image* metallic = nullptr;
		std::string metallicPath;
		Data::Image* roughness = nullptr;
		std::string roughnessPath;
		Data::Image* ao = nullptr;
		std::string aoPath;
	};
}