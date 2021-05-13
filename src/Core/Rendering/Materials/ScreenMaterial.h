#pragma once
#include "Core/Rendering/Material.h"
#include <Core/TypeRegister.h>

namespace Tristeon::Core::Rendering
{
	class ScreenMaterial : public Material
	{
		REGISTER_TYPE_H(ScreenMaterial);
	public:
		ScreenMaterial() = default;
		explicit ScreenMaterial(PipelineProperties pProperties) : Material(pProperties) {}

		ShaderFile* shader() override;
		void createPipeline() override;
		void createDescriptorSets() override;
	};
}