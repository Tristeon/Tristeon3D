#include "ForwardVulkan.h"

#include <glm/gtc/matrix_transform.inl>

#include "Core/Rendering/Vulkan/RenderManagerVulkan.h"
#include "InternalMeshRendererVulkan.h"

#include "HelperClasses/Pipeline.h"
#include "HelperClasses/CameraRenderData.h"
#include "HelperClasses/EditorGrid.h"
#include "HelperClasses/Swapchain.h"
#include "DebugDrawManagerVulkan.h"

namespace Tristeon
{
	namespace Core
	{
		namespace Rendering
		{
			namespace Vulkan
			{
				Forward::Forward(Rendering::RenderManager* renderManager): RenderTechnique(renderManager)
				{
					//Store derived object for ease of use
					vkRenderManager = dynamic_cast<RenderManager*>(renderManager);
				}

				void Forward::renderScene(glm::mat4 view, glm::mat4 proj, TObject* info)
				{
					//if our camera is null, try and see if we can find it in info
					CameraRenderData* d = dynamic_cast<CameraRenderData*>(info);
					vk::CommandBufferBeginInfo cmdBegin = vk::CommandBufferBeginInfo();

					//Clear data
					std::array<float, 4> const background = { 0.1f, 0.1f, 0.1f, 1 };
					vk::ClearValue clear[2];
					clear[0].color = vk::ClearColorValue(background);
					clear[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

					//Get extent
					vk::Extent2D const extent = vkRenderManager->swapchain->extent2D;
					vk::Rect2D const renderArea = vk::Rect2D(vk::Offset2D(0, 0), extent);

					//Begin renderpass
					vk::RenderPassBeginInfo renderPassBegin = vk::RenderPassBeginInfo(d->offscreen.pass, d->offscreen.buffer, renderArea, 2, clear);

					//Begin primary
					vk::CommandBuffer primary = d->offscreen.cmd;
					primary.begin(&cmdBegin);
					primary.beginRenderPass(&renderPassBegin, vk::SubpassContents::eSecondaryCommandBuffers);

					std::vector<vk::CommandBuffer> buffers;
					int const index = vkRenderManager->index;

					//Setup renderdata
					vk::CommandBufferInheritanceInfo const inheritance = vk::CommandBufferInheritanceInfo(d->offscreen.pass, 0, d->offscreen.buffer); //ignore query
					RenderData data;
					data.inheritance = inheritance;
					data.index = index;
					data.viewport = vk::Viewport(0, 0, extent.width, extent.height, 0, 1.0f);
					data.scissor = vk::Rect2D({ 0, 0 }, extent);
					data.primary = primary;
					data.projection = proj;
					data.view = view;

#ifdef EDITOR
					//Draw grid
					if (!vkRenderManager->inPlayMode)
					{
						data.lastUsedSecondaryBuffer = nullptr;

						vkRenderManager->grid->renderer->data = &data;
						vkRenderManager->grid->renderer->render();

						if ((VkCommandBuffer)data.lastUsedSecondaryBuffer != nullptr)
							buffers.push_back(data.lastUsedSecondaryBuffer);
					}
#endif

					//Draw debug data
					{
						data.lastUsedSecondaryBuffer = nullptr;
						Vulkan::DebugDrawManager* ddmngr = (Vulkan::DebugDrawManager*)DebugDrawManager::instance;
						ddmngr->data = &data;
						ddmngr->draw();
						if ((VkCommandBuffer)data.lastUsedSecondaryBuffer != nullptr)
							buffers.push_back(data.lastUsedSecondaryBuffer);
					}

					//Draw scene
					for (InternalMeshRenderer* r : vkRenderManager->internalRenderers)
					{
						data.lastUsedSecondaryBuffer = nullptr;

						r->data = &data;
						r->render();

						if ((VkCommandBuffer)data.lastUsedSecondaryBuffer != nullptr)
							buffers.push_back(data.lastUsedSecondaryBuffer);
					}

					//Execute and finish
					if (buffers.size() != 0)
						primary.executeCommands(buffers.size(), buffers.data());
					primary.endRenderPass();
					primary.end();
				}

				void Forward::renderCameras()
				{
					//Swapchain framebuffer
					vk::Framebuffer const fb = vkRenderManager->getActiveFrameBuffer();

					//Clear data
					std::array<float, 4> const background = { 0, 0, 0, 1 };
					vk::ClearValue clear[2];
					clear[0].color = vk::ClearColorValue(background);
					clear[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

					//Screen area
					vk::Extent2D const extent = vkRenderManager->swapchain->extent2D;
					vk::Rect2D const renderArea = vk::Rect2D(vk::Offset2D(0, 0), extent);

					//Begin commandbuffer
					vk::CommandBufferBeginInfo cmdBegin = vk::CommandBufferBeginInfo();
					vk::CommandBuffer primary = vkRenderManager->primaryBuffers[vkRenderManager->index];
					primary.begin(&cmdBegin);
	
					//Begin renderpass
					vk::RenderPassBeginInfo renderPassBegin = vk::RenderPassBeginInfo(
						vkRenderManager->swapchain->renderpass, fb,
						renderArea, 2, clear);
					primary.beginRenderPass(&renderPassBegin, vk::SubpassContents::eSecondaryCommandBuffers);

					//Store secondary buffers, submit in bulk afterwards
					std::vector<vk::CommandBuffer> buffers;
					int const index = vkRenderManager->index;

					//Get data
					vk::CommandBufferInheritanceInfo const inheritance = vk::CommandBufferInheritanceInfo(vkRenderManager->swapchain->renderpass, 0, fb); //ignore query
					RenderData data;
					data.inheritance = inheritance;
					data.index = index;
					data.viewport = vk::Viewport(0, 0, extent.width, extent.height, 0, 1.0f);
					data.scissor = vk::Rect2D({ 0, 0 }, extent);
					data.primary = primary;

					//Only draw cameras in playmode
					if (vkRenderManager->inPlayMode)
					{
						//Draw cameras
						for (auto const p : vkRenderManager->cameraData)
						{
							CameraRenderData* d = p.second;

							d->onscreen.secondary.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritance));
							d->onscreen.secondary.setViewport(0, 1, &data.viewport);
							d->onscreen.secondary.setScissor(0, 1, &data.scissor);
							d->onscreen.secondary.bindPipeline(vk::PipelineBindPoint::eGraphics, vkRenderManager->onscreenPipeline->getPipeline());
							d->onscreen.secondary.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vkRenderManager->onscreenPipeline->getPipelineLayout(), 0, 1, &d->onscreen.set, 0, nullptr);
							d->onscreen.secondary.draw(3, 1, 0, 0);
							d->onscreen.secondary.end();
							buffers.push_back(d->onscreen.secondary);
						}
					}

					//Draw UI
					for (UIRenderable* r : vkRenderManager->renderables)
					{
						data.lastUsedSecondaryBuffer = nullptr;
						r->data = &data;
						r->onRender.invoke();
						if (data.lastUsedSecondaryBuffer)
							buffers.push_back(data.lastUsedSecondaryBuffer);
					}

					//Execute
					if (buffers.size() != 0)
						primary.executeCommands(buffers.size(), buffers.data());

					//End
					primary.endRenderPass();
					primary.end();
				}
			}
		}
	}
}
