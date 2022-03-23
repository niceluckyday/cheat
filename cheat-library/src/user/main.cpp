#include "pch-il2cpp.h"
#include "main.h"

#include <filesystem>
#include <string>
#include <iostream>

#include <magic_enum.hpp>

#include <gcclib/Logger.h>
#include <gcclib/Globals.h>

#include <helpers.h>
#include <il2cpp-init.h>
#include <gui/renderer.h>
#include <cheat/cheat.h>
#include <common/config/Config.h>
#include <cheat/misc/Settings.h>

void Run(HMODULE* phModule)
{
    Sleep(4000);

    init_il2cpp();
    il2cpp_thread_attach(il2cpp_domain_get());

    // Init config
    std::string configPath = (std::filesystem::current_path() / Globals::configFileName).string();
    config::Init(configPath);

    // Init logger
    auto& settings = cheat::feature::Settings::GetInstance();
    if (settings.m_FileLogging)
    {
        Logger::PrepareFileLogging((std::filesystem::current_path() / "logs").string());
        Logger::SetLevel(Logger::Level::Trace, Logger::LoggerType::FileLogger);
    }

    if (settings.m_ConsoleLogging)
    {
        Logger::SetLevel(Logger::Level::Debug, Logger::LoggerType::ConsoleLogger);
        il2cppi_new_console();
    }

    LOG_DEBUG("Config path is %s", configPath.c_str());
    LOG_DEBUG("UserAssembly.dll at 0x%p", il2cppi_get_base_address());
    LOG_DEBUG("UnityPlayer.dll  at 0x%p", il2cppi_get_unity_address());

    cheat::Init(*phModule);
}