#include "pch-il2cpp.h"
#include "GenshinMisc.h"

cheat::GenshinMisc& cheat::GenshinMisc::GetInstance()
{
	static GenshinMisc instance;
	return instance;
}

void cheat::GenshinMisc::CursorSetVisibility(bool visibility)
{
	app::Cursor_set_visible(nullptr, visibility, nullptr);
	app::Cursor_set_lockState(nullptr, visibility ? app::CursorLockMode__Enum::None : app::CursorLockMode__Enum::Locked, nullptr);
}

bool cheat::GenshinMisc::CursorGetVisibility()
{
    return app::Cursor_get_visible(nullptr, nullptr);
}
