#ifdef TRISTEON_EDITOR

#include "EditorSelection.h"
#include "Selectable.h"

using namespace Tristeon::Editor;

Selectable* EditorSelection::selectedItem = nullptr;

Selectable* EditorSelection::getSelectedItem()
{
	return selectedItem;
}

void EditorSelection::setSelectedItem(Selectable* node)
{
	selectedItem = node;
}

#endif