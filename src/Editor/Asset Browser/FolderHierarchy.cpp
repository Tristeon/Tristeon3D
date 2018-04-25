#if TRISTEON_EDITOR

#include "FolderHierarchy.h"
#include "FileItemManager.h"
#include "FolderItem.h"
#include <iostream>
#include "Editor/EditorDragging.h"

using namespace Tristeon::Editor;

FolderHierarchy::FolderHierarchy()
{
}

FolderHierarchy::~FolderHierarchy()
{
}

void FolderHierarchy::drawHierarchy(FolderItem* rootFolder, FileItemManager* itemManager)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	const bool nodeOpen = ImGui::TreeNodeEx("Assets",flags);

	//Is the user dragging a fileitem?
	FileItem* draggingItem = dynamic_cast<FileItem*>(EditorDragging::getDragableItem());
	//Is the user dropping the item on here?
	if (draggingItem != nullptr && ImGui::IsItemHovered()) {
		std::cout << "Dropped: " << draggingItem->name << "\tOn: Assets" << std::endl;
		draggingItem->move(rootFolder);
	}

	//When folder is clicked, view its fileitems that it contains
	if (ImGui::IsItemClicked()) itemManager->setView(rootFolder);
	if (nodeOpen)
	{
		rootFolder->drawHierarchy(itemManager);
		ImGui::TreePop();
	}
}

#endif