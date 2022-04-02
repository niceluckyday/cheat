#include "pch-il2cpp.h"
#include "main.h"

#include <helpers.h>
#include <il2cpp-init.h>
#include <cheat/cheat.h>
#include <cheat-base/cheat/misc/Settings.h>

#include <cheat/ILPatternScanner.h>
#include <resource.h>
#include <cheat-base/config/field/StringField.h>

bool StubTerminateProcess();

void Run(HMODULE* phModule)
{
    Sleep(4000);

	// Init config
	std::string configPath = (std::filesystem::current_path() / "cfg.ini").string();
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

    // Getting signatures data from resources
	LPBYTE pSignaturesData = nullptr;
	DWORD signaturesSize = 0;
	if (!util::GetResourceMemory(*phModule, IDR_RCDATA2, pSignaturesData, signaturesSize))
	{
		LOG_LAST_ERROR("Failed load signatures resource.");
		return;
	}

    std::string signaturesContent = std::string((char*)pSignaturesData, signaturesSize);

    // Initializing all functions
    init_il2cpp(signaturesContent);

	if (StubTerminateProcess())
		LOG_INFO("TerminateProcess stubbed successfully.");
	else
		LOG_ERROR("Stub TerminateProcess failed.");

	cheat::Init(*phModule);

    LOG_DEBUG("Config path is %s", configPath.c_str());
    LOG_DEBUG("UserAssembly.dll at 0x%p", il2cppi_get_base_address());
    LOG_DEBUG("UnityPlayer.dll  at 0x%p", il2cppi_get_unity_address());
}

BOOL WINAPI TerminateProcess_Hook(HANDLE hProcessUINT, UINT uExitCode)
{
    return TRUE;
}

bool StubTerminateProcess()
{
    HMODULE hKernelBase = GetModuleHandle("kernelbase.dll");
    if (hKernelBase == NULL)
    {
        LOG_LAST_ERROR("Getting kernelbase.dll handle failed.");
        return false;
    }

    FARPROC pTerminateProcess = GetProcAddress(hKernelBase, "TerminateProcess");
    if (pTerminateProcess == nullptr)
    {
        LOG_LAST_ERROR("Getting KernelBase::NtTerminateProcess failed.");
        return false;
    }
    using TerminateProcess_Type = BOOL(*)(HANDLE, UINT);
    
    HookManager::install((TerminateProcess_Type)pTerminateProcess, TerminateProcess_Hook);
    LOG_DEBUG("Terminate process hooked. Origin at 0x%p", HookManager::getOrigin(TerminateProcess_Hook));
    return true;
}