#include "pch-il2cpp.h"
#include "main.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <filesystem>
#include <string>
#include <iostream>

#include <magic_enum.hpp>
#include "il2cpp-init.h"
#include "helpers.h"

#include "util/Config.h"
#include "util/Logger.h"

#include "debug-hooks.h"
#include "protection-bypass.h"

#include "gui/renderer.h"
#include "cheat/cheat.h"

const char* INIFileName = "cfg.ini";

void Run(HMODULE* phModule)
{
    Sleep(4000);
    
    init_il2cpp();
    il2cpp_thread_attach(il2cpp_domain_get());

    // Init config
    std::string configPath = (std::filesystem::current_path() / INIFileName).string();
    Config::Init(configPath);

    // Init logger
    if (Config::cfgFileLogEnabled.GetValue()) 
    {
        Logger::PrepareFileLogging((std::filesystem::current_path() / "logs").string());
        Logger::SetLevel(Logger::Level::Trace, Logger::LoggerType::FileLogger);
    }

    if (Config::cfgConsoleLogEnabled.GetValue()) 
    {
        Logger::SetLevel(Logger::Level::Debug, Logger::LoggerType::ConsoleLogger);
        il2cppi_new_console();
    }
        

    LOG_DEBUG("Config path is %s", configPath.c_str());
    LOG_DEBUG("UserAssembly.dll at 0x%p", il2cppi_get_base_address());
    LOG_DEBUG("UnityPlayer.dll  at 0x%p", il2cppi_get_base_address());

    InitProtectionBypass();
    InitDebugHooks();
    InitCheats();

    InitRenderer(*phModule);
}