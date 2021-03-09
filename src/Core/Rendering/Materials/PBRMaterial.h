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
		enum class TextureType
		{
			Albedo,
			Normal,
			Metallic,
			Roughness,
			Ao,
			EndOfEnum
		};

		PBRMaterial() = default;
		virtual ~PBRMaterial() override;
		explicit PBRMaterial(PipelineProperties pProperties);

		ShaderFile* shader() override;
		void createPipeline() override;
		void createDescriptorSets() override;

		Data::Image* texture(TextureType type) const;

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;
	private:
		Data::Image* _albedo = nullptr;
		std::string _albedoPath;
		Data::Image* _normal = nullptr;
		std::string _normalPath;
		Data::Image* _metallic = nullptr;
		std::string _metallicPath;
		Data::Image* _roughness = nullptr;
		std::string _roughnessPath;
		Data::Image* _ao = nullptr;
		std::string _aoPath;
	};
}