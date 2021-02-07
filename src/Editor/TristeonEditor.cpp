#include "Core/BindingData.h"
#include "Core/MessageBus.h"
#ifdef TRISTEON_EDITOR

#include "TristeonEditor.h"
#include <ImGUI/imgui_impl_glfw_vulkan.h>
#include "Core/Engine.h"
#include "Core/Rendering/Vulkan/HelperClasses/CommandBuffer.h"
#include "Core/Rendering/Vulkan/RenderManagerVulkan.h"
#include "Asset Browser/AssetBrowser.h"
#include "Scene editor/GameObjectHierarchy.h"
#include "Inspector/InspectorWindow.h"
#include "Scene editor/SceneWindow.h"
#include "Misc/Console.h"
#include "Misc/Hardware/Keyboard.h"
#include "EditorDragging.h"

namespace Tristeon
{
	using namespace Editor;

	TristeonEditor::TristeonEditor(Core::Engine* engine)
	{
		editorCamera = nullptr;

		this->vkDevice = Core::binding_data.device;
		this->engine = engine;

		bindImGui();
		initFontsImGui();
		setupCallbacks();
		createCommandBuffers();

		//Set style
		setStyle();

		//Create editor windows
		EditorWindow::editor = this;
		windows.push_back(std::move(std::make_unique<AssetBrowser>()));
		windows.push_back(std::move(std::make_unique<GameObjectHierarchy>()));
		windows.push_back(std::move(std::make_unique<InspectorWindow>()));
		windows.push_back(std::move(std::make_unique<SceneWindow>(this)));
	}

	TristeonEditor::~TristeonEditor()
	{
		Core::MessageBus::sendMessage(Core::Message(Core::MT_RENDERINGCOMPONENT_DEREGISTER, renderable));
		delete renderable;

		vkDevice.waitIdle();
		ImGui_ImplGlfwVulkan_Shutdown();
	}

	void TristeonEditor::setStyle()
	{
		ImGuiStyle * style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0, 0, 0, 0);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.301f, 0.537f, .788f, 1);
		style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
		style->Colors[ImGuiCol_TSelectableActive] = ImVec4(1,1,1,1);
	}

	void TristeonEditor::onGui()
	{
		if (Misc::Keyboard::getKeyDown(Misc::KeyCode::SPACE))
		{
			inPlayMode = !inPlayMode;
			Core::MessageBus::sendMessage(inPlayMode ? Core::MT_GAME_LOGIC_START : Core::MT_GAME_LOGIC_STOP);
		}

		//If playing the game it will go in fullscreen, thus the editor no longer needs to be rendered, will be subject to change
		if (inPlayMode)
			return;

		ImGui_ImplGlfwVulkan_NewFrame();
		//ImGuizmo::BeginFrame();

		//Calling ongui functions of editorwindows
		for (int i = 0; i < windows.size(); ++i)
		{
			windows[i]->onGui();
		}

		//Update dragging
		if (ImGui::IsMouseReleased(0))
		{
			EditorDragging::reset();
		}
	}

	void TristeonEditor::render()
	{
		if (inPlayMode)
			return;

		Core::Rendering::Vulkan::RenderData* d = dynamic_cast<Core::Rendering::Vulkan::RenderData*>(renderable->data);

		vk::CommandBufferBeginInfo const begin = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &d->inheritance);
		cmd.begin(begin);
		ImGui_ImplGlfwVulkan_Render(static_cast<VkCommandBuffer>(cmd));
		cmd.end();

		d->lastUsedSecondaryBuffer = cmd;
	}

	void TristeonEditor::bindImGui()
	{
		ImGui_ImplGlfwVulkan_Init_Data init_data;
		init_data.allocator = nullptr;
		init_data.gpu = static_cast<VkPhysicalDevice>(Core::binding_data.physical);
		init_data.device = static_cast<VkDevice>(Core::binding_data.device);
		init_data.render_pass = static_cast<VkRenderPass>(Core::binding_data.main_pass);
		init_data.pipeline_cache = NULL;
		init_data.descriptor_pool = static_cast<VkDescriptorPool>(Core::binding_data.descriptorPool);
		init_data.check_vk_result = [](VkResult err) { Misc::Console::t_assert(err == VK_SUCCESS, "Editor vulkan error: " + err); };
		ImGui_ImplGlfwVulkan_Init(Core::binding_data.window, true, &init_data);
	}

	void TristeonEditor::initFontsImGui()
	{
		VkResult err = vkResetCommandPool(
			static_cast<VkDevice>(Core::binding_data.device),
			static_cast<VkCommandPool>(Core::binding_data.commandPool),
			0);

		Misc::Console::t_assert(err == VK_SUCCESS, "Failed to reset command pool: " + to_string(static_cast<vk::Result>(err)));
		VkCommandBuffer const cmd = static_cast<VkCommandBuffer>(Core::Rendering::Vulkan::CommandBuffer::begin());

		ImGui_ImplGlfwVulkan_CreateFontsTexture(cmd);
		Core::Rendering::Vulkan::CommandBuffer::end(static_cast<vk::CommandBuffer>(cmd));

		Core::binding_data.device.waitIdle();
		ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
	}

	void TristeonEditor::setupCallbacks()
	{
		//Subscribe to render callback
		Core::MessageBus::subscribeToMessage(Core::MT_PRERENDER, std::bind(&TristeonEditor::onGui, this));
		Core::MessageBus::subscribeToMessage(Core::MT_SHARE_DATA, [&](Core::Message msg)
		{
			Core::Rendering::Vulkan::EditorData* data = dynamic_cast<Core::Rendering::Vulkan::EditorData*>(msg.userData);
			if (data != nullptr)
				this->editorCamera = data;
		});

		renderable = new Core::Rendering::UIRenderable();
		renderable->onRender += [&]() { render(); };
		Core::MessageBus::sendMessage(Core::Message(Core::MT_RENDERINGCOMPONENT_REGISTER, renderable));
	}

	void TristeonEditor::createCommandBuffers()
	{
		vk::CommandBufferAllocateInfo alloc = vk::CommandBufferAllocateInfo(Core::binding_data.commandPool, vk::CommandBufferLevel::eSecondary, 1);
		vk::Result const r = Core::binding_data.device.allocateCommandBuffers(&alloc, &cmd);
		Misc::Console::t_assert(r == vk::Result::eSuccess, "Failed to allocate command buffers: " + to_string(r));
	}
}

#endif