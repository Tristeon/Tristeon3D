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
		virtual ~ScreenMaterial() override;
		ScreenMaterial(PipelineProperties properties) : Material(properties) {}

		ShaderFile* shader() override;
		void createPipeline() override;
		void createDescriptorSets() override;

	protected:
		void render(vk::CommandBuffer cmd) override;
	private:
		vk::DescriptorSetLayout _setLayout;
		vk::DescriptorSet _set;
	};
}