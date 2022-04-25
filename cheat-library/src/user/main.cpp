#include "pch-il2cpp.h"
#include "main.h"

#include <helpers.h>
#include <il2cpp-init.h>
#include <cheat/cheat.h>
#include <cheat-base/cheat/misc/Settings.h>

#include <cheat/ILPatternScanner.h>
#include <resource.h>
#include <cheat-base/config/field/StringField.h>

bool InitMetadata();
bool StubTerminateProcess();

void Run(HMODULE* phModule)
{
    Sleep(10000);

	ResourceLoader::SetModuleHandle(*phModule);

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

	if (InitMetadata())
	{
		LOG_INFO("Metadata was successfully initialized.");
	}
	else
	{
		LOG_CRIT("Failed to initialize metadata. Please contact the developers about this problem");
		return;
	}

	if (StubTerminateProcess())
		LOG_INFO("TerminateProcess stubbed successfully.");
	else
		LOG_ERROR("Stub TerminateProcess failed.");

	cheat::Init();

    LOG_DEBUG("Config path is at %s", configPath.c_str());
    LOG_DEBUG("UserAssembly.dll is at 0x%p", il2cppi_get_base_address());
    LOG_DEBUG("UnityPlayer.dll is at 0x%p", il2cppi_get_unity_address());
}

bool InitMetadata()
{

	// Getting signatures data from resources
	LPBYTE pSignaturesData = nullptr;
	DWORD signaturesSize = 0;
	if (!ResourceLoader::LoadEx(IDR_RCDATA2, RT_RCDATA, pSignaturesData, signaturesSize))
	{
		LOG_LAST_ERROR("Failed to load signatures resource.");
		return false;
	}

	std::string signaturesContent = std::string((char*)pSignaturesData, signaturesSize);

	// Getting cached offsets
	LPBYTE pCachedOffsetsData = nullptr;
	DWORD cachedOffsetsSize = 0;
	if (!ResourceLoader::LoadEx(IDR_RCDATA3, RT_RCDATA, pCachedOffsetsData, cachedOffsetsSize))
	{
		LOG_LAST_ERROR("Failed to load cached offsets resource.");
		return false;
	}
	std::string cachedOffsets = std::string((char*)pCachedOffsetsData, cachedOffsetsSize);

	// Initializing all functions
	init_il2cpp(signaturesContent, cachedOffsets);

	return true;
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
        LOG_LAST_ERROR("Failed to get the kernelbase.dll handle.");
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