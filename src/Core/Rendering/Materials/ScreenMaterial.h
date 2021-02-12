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
		ScreenMaterial(PipelineProperties properties) : Material(properties) {}

		ShaderFile* shader() override;
		void buildPipeline() override;
	};
}