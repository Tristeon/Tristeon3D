#if TRISTEON_EDITOR

#include "EditorWindow.h"
#include "TristeonEditor.h"

using namespace Tristeon::Editor;

TristeonEditor* EditorWindow::editor = nullptr;

EditorWindow::EditorWindow()
{
	windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders;
}


EditorWindow::~EditorWindow()
{
}

#endif