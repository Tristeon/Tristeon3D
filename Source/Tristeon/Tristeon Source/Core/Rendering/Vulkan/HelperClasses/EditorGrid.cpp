#include "EditorGrid.h"
#include "Data/Mesh.h"
#include "Pipeline.h"
#include "Core/Rendering/Vulkan/MaterialVulkan.h"
#include "Core/GameObject.h"
#include <vulkan/vulkan.hpp>
#include "Core/Rendering/Components/MeshRenderer.h"
#include <Core/Rendering/Vulkan/InternalMeshRendererVulkan.h>
#include "Core/BindingData.h"

Tristeon::Core::Rendering::Vulkan::EditorGrid::EditorGrid(VulkanBindingData* data, vk::RenderPass offscreenPass)
{
	this->data = data;

	//ShaderFile
	file = ShaderFile("Line", "Files/Shaders/", "LineV", "LineF");
	//Pipeline
	pipeline = new Pipeline(data, file, data->swapchain->extent2D, offscreenPass, true, vk::PrimitiveTopology::eLineList);

	//Material
	material = new Material();
	material->pipeline = pipeline;
	material->setupTextures();
	material->createDescriptorSets();

	//Generate mesh
	const int lines = 20;
	Data::SubMesh mesh;
	for (int i = -lines; i < lines; i++)
	{
		Data::Vertex v;
		v.pos = glm::vec3(i, 0, lines - 1);

		Data::Vertex v2;
		v2.pos = glm::vec3(i, 0, -lines);

		mesh.indices.push_back(mesh.vertices.size());
		mesh.vertices.push_back(v);

		mesh.indices.push_back(mesh.vertices.size());
		mesh.vertices.push_back(v2);
	}

	for (int i = -lines; i < lines; i++)
	{
		Data::Vertex v;
		v.pos = glm::vec3(lines - 1, 0, i);

		Data::Vertex v2;
		v2.pos = glm::vec3(-lines, 0, i);

		mesh.indices.push_back(mesh.vertices.size());
		mesh.vertices.push_back(v);

		mesh.indices.push_back(mesh.vertices.size());
		mesh.vertices.push_back(v2);
	}

	//Create object and meshrenderer
	object = new GameObject(false); //Unregistered gameobject
	mr = object->addComponent<MeshRenderer>();
	mr->mesh = mesh;
	mr->material = material;
	mr->initInternalRenderer((BindingData*)data);

	//Get internal renderer
	renderer = dynamic_cast<InternalMeshRenderer*>(mr->getInternalRenderer());
}

Tristeon::Core::Rendering::Vulkan::EditorGrid::~EditorGrid()
{
	//Cleanup
	delete material;
	delete pipeline;
	delete object;
}

void Tristeon::Core::Rendering::Vulkan::EditorGrid::rebuild(vk::RenderPass offscreenPass) const
{
	//Rebuild pipeline and renderer
	pipeline->rebuild(data->swapchain->extent2D, offscreenPass);
}
