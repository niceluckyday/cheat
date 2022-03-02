#include "pch-il2cpp.h"
#include "main.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <filesystem>
#include <string>
#include <iostream>

#include "il2cpp-init.h"
#include "helpers.h"

#include "debug-hooks.h"
#include "protection-bypass.h"
#include "util/Config.h"
#include "gui/renderer.h"
#include "cheat/cheat.h"

const char* INIFileName = "cfg.ini";

void Run(HMODULE* phModule)
{
    Sleep(4000); // Waiting for il2cpp initialize
    
    init_il2cpp();

    il2cpp_thread_attach(il2cpp_domain_get());

    std::string configPath = (std::filesystem::current_path() / INIFileName).string();
    Config::Init(configPath);
    if (Config::cfgConsoleLogging.GetValue())
        il2cppi_new_console();

    std::cout << "Config path is " << (std::filesystem::current_path() / INIFileName).string() << std::endl;
    std::cout << "UserAssembly.dll at 0x" << il2cppi_get_base_address() << std::endl;
    std::cout << "UnityPlayer.dll  at 0x" << il2cppi_get_unity_address() << std::endl;

    InitProtectionBypass();

    InitDebugHooks();

    InitMapTPHooks();

    InitRenderer(*phModule);
}