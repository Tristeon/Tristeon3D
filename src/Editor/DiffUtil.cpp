#if TRISTEON_EDITOR

#include "DiffUtil.h"
#include "Core/GameObject.h"

void Tristeon::Editor::DiffUtil::takeSnapshot(std::shared_ptr<Core::GameObject> object)
{
	currentSnapshot = object;
}

void Tristeon::Editor::DiffUtil::commitChanges(std::string description)
{
	if (currentSnapshot == nullptr) return;
	snapshots[description] = currentSnapshot;
}

#endif