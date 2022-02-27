#include "pch-il2cpp.h"
#include "renderer.h"

#include <iostream>

#include "util/dx11hook.h"
#include "imgui.h"

static bool isShowMain = false;
static bool isBlocked = false;

void GuiRender();
void OnKeyUp(WPARAM key, short& ioFlag);

void InitRenderer()
{
	std::cout << "[Renderer] Creating overlay..." << std::endl;
	createOverlay(GuiRender, OnKeyUp, nullptr, 0);
}

void GuiRender() {
	if (!isShowMain)
		return;

	ImGui::ShowDemoWindow();
}

void OnKeyUp(WPARAM key, short& ioFlag) {
	if (key == VK_F1) {
		isShowMain = !isShowMain;
	}

	ioFlag = 0;
	if (isShowMain) {
		ioFlag |= BType_SendImGUI;
		if (isBlocked)
			ioFlag |= BType_Blocked;
	}
}