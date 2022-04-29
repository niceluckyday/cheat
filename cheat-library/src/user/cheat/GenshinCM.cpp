#include "pch-il2cpp.h"
#include "GenshinCM.h"

cheat::GenshinCM& cheat::GenshinCM::instance()
{
	static GenshinCM instance;
	return instance;
}

void cheat::GenshinCM::CursorSetVisibility(bool visibility)
{
	app::Cursor_set_visible(nullptr, visibility, nullptr);
	app::Cursor_set_lockState(nullptr, visibility ? app::CursorLockMode__Enum::None : app::CursorLockMode__Enum::Locked, nullptr);
}

bool cheat::GenshinCM::CursorGetVisibility()
{
	return app::Cursor_get_visible(nullptr, nullptr);
}

void cheat::GenshinCM::DrawProfileLine()
{
	CheatManagerBase::DrawProfileLine();
}
