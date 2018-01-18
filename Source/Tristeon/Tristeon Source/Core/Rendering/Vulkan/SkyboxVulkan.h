#pragma once
#include "Core/Rendering/Skybox.h"
#include "HelperClasses/VulkanImage.h"
#include "RenderManagerVulkan.h"
#include "MaterialVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		class VulkanBindingData;
		namespace Rendering
		{
			namespace Vulkan
			{
				class RenderManager;

				class Skybox : public Rendering::Skybox
				{
					friend Vulkan::Forward;
					friend RenderManager;

				public:
					explicit Skybox(VulkanBindingData* bindingData, vk::RenderPass renderPass) : bindingData(bindingData), renderPass(renderPass) { }
					~Skybox();
				
				protected:
					void init() override;
					void draw(glm::mat4 view, glm::mat4 proj) override;

				private:
					void rebuild(vk::Extent2D extent, vk::RenderPass renderPass);

					void setupCubemap();
					void setupPipeline();
					void createUniformBuffer();
					void createVertexBuffer();
					void createIndexBuffer();
					void createDescriptorSet();
					void createCommandBuffers();

					VulkanBindingData* bindingData = nullptr;
					vk::RenderPass renderPass = nullptr;

					RenderData* data = nullptr;

					struct Image
					{
						vk::Image img;
						vk::DeviceMemory mem;
						vk::ImageView view;
						vk::Sampler sampler;
						vk::DescriptorSet set;
					} image;

					UniformBufferObject ubo;
					struct Buffer
					{
						vk::Buffer buf;
						vk::DeviceMemory mem;
					};
					Buffer uniformBuffer;
					Buffer vertexBuffer;
					Buffer indexBuffer;
					vk::CommandBuffer secondary;
					Pipeline* pipeline = nullptr;
				};
			}
		}
	}
}