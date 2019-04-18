#pragma once
#include "Core/Rendering/Skybox.h"
#include "MaterialVulkan.h"
#include "RenderManagerVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		class VulkanBindingData;
		namespace Rendering
		{
			namespace Vulkan
			{
				class Forward;
				class RenderManager;
				
				class Skybox : public Rendering::Skybox
				{
					friend Vulkan::Forward;
					friend RenderManager;

					struct Image
					{
						vk::Image img;
						vk::DeviceMemory mem;
						vk::ImageView view;
						vk::Sampler sampler;
						vk::DescriptorSet set;
					};

				public:
					explicit Skybox(vk::RenderPass renderPass) : renderPass(renderPass) { }
					~Skybox();
				
					const Image& getImage() const { return image; }
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
					void createOffscreenDescriptorSet();
					void createCommandBuffers();

					vk::RenderPass renderPass = nullptr;

					RenderData* data = nullptr;

					Image image;

					UniformBufferObject ubo;
			
					std::unique_ptr<BufferVulkan> uniformBuffer;
					std::unique_ptr<BufferVulkan> vertexBuffer;
					std::unique_ptr<BufferVulkan> indexBuffer;

					vk::CommandBuffer secondary;
					Pipeline* pipeline = nullptr;

					vk::DescriptorSet lightingSet;
				};
			}
		}
	}
}
