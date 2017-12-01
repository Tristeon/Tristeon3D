#include "EditorWindow.h"

using namespace Tristeon::Editor;

EditorWindow::EditorWindow()
{
	windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders;
}


EditorWindow::~EditorWindow()
{
}