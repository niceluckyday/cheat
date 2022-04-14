#pragma once

#include <string>

#include <il2cpp-appdata.h>
#include <imgui.h>

#include <cheat/game/Entity.h>

namespace cheat::feature::esp::render
{
	void DrawEntity(const std::string& name, game::Entity* entity, const ImColor& color);
	void PrepareFrame();
}