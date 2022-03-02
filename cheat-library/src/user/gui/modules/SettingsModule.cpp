#include "pch-il2cpp.h"
#include "SettingsModule.h"

#include <imgui.h>

#include <il2cpp-appdata.h>
#include <helpers.h>

#include <gui/gui-util.h>
#include <util/Config.h>

void SettingsModule::Draw()
{
    bool consoleChanged = ConfigWidget("Enable console", Config::cfgConsoleLogging, 
        "Enables console for logging information. (Enabling will take effect after next launch)");
    if (consoleChanged && !Config::cfgConsoleLogging.GetValue()) {
        il2cppi_close_console();
    }

    ConfigWidget("Disable mhyprot on launch", Config::cfgDisableMhyProt,
        "Closing handle of mhyprot2.sys driver. (Changes will take effect after next launch)\n" \
        "It allows reading / writing process memory from another process.\n" \
        "For example, this allows Cheat engine read process.");

    ConfigWidget("Toggle menu key", Config::cfgMenuEnableKey, false,
        "Key to toggle this menu visibility. Cannot be empty.\nIf you forget this key, you can see it in config file.");
}

std::string SettingsModule::GetName()
{
	return "Settings";
}

bool SettingsModule::CanBeUnpinned()
{
	return false;
}
