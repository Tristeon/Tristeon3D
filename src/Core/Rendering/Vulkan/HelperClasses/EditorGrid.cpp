#include "EditorGrid.h"
#include "Data/Mesh.h"
#include "Pipeline.h"
#include "Core/Rendering/Vulkan/MaterialVulkan.h"
#include "Core/GameObject.h"
#include <vulkan/vulkan.hpp>
#include "Core/Rendering/Components/MeshRenderer.h"
#include <Core/Rendering/Vulkan/InternalMeshRendererVulkan.h>
#include "Core/BindingData.h"

Tristeon::Core::Rendering::Vulkan::EditorGrid::EditorGrid(vk::RenderPass offscreenPass)
{
	//Editor grid basically simulates a grid by taking a material, pipeline and meshrenderer,
	//setting the rendermode to linelist, and generating a mesh existing of lines

	//Set up our pipeline
	file = ShaderFile("Line", "Files/Shaders/", "LineV", "LineF");
	pipeline = new Pipeline(file, binding_data.extent, offscreenPass, true, vk::PrimitiveTopology::eLineList);

	//Set up our material with our shader pipeline
	material = new Material();
	material->shader = std::make_unique<ShaderFile>(file);
	material->pipeline = pipeline;
	material->updateProperties(true);
	material->setColor("Color.color", Misc::Color(1, 1, 1, 1));

	//Create the mesh, existing of 20x20 lines
	const int lines = 20;
	Data::SubMesh mesh;

	//Z lines
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

	//X lines
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
	object = new GameObject(); //Unregistered gameobject
	object->name = "EditorGrid";
	mr = object->addComponent<MeshRenderer>();
	mr->mesh = mesh;
	mr->material = material;
	mr->initInternalRenderer();

	//Get internal renderer
	renderer = dynamic_cast<InternalMeshRenderer*>(mr->getInternalRenderer());
}

Tristeon::Core::Rendering::Vulkan::EditorGrid::~EditorGrid()
{
	//Cleanup our allocated resources
	delete material;
	delete pipeline;
	delete object;
}

void Tristeon::Core::Rendering::Vulkan::EditorGrid::rebuild(vk::RenderPass offscreenPass) const
{
	//The pipeline has to be rebuilt the moment the swapchain changes,
	//to fit the new swapchain/window size
	pipeline->rebuild(binding_data.extent, offscreenPass);
}
