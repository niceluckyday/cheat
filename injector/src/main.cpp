#include <Windows.h>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <gcclib/simple-ini.hpp>
#include <gcclib/Globals.h>

#include "injector.h"
#include "util.h"

const std::string GlobalGenshinProcName = "GenshinImpact.exe";
const std::string ChinaGenshinProcName = "YuanShen.exe";


static CSimpleIni ini;

HANDLE OpenGenshinProcess();

int main(int argc, char* argv[])
{
    Logger::SetLevel(Logger::Level::Debug, Logger::LoggerType::ConsoleLogger);

    auto path = std::filesystem::path(argv[0]).parent_path();
    std::filesystem::current_path(path);
    
    WaitForCloseProcess(GlobalGenshinProcName);
    WaitForCloseProcess(ChinaGenshinProcName);

    Sleep(1000); // Wait for unloading all dlls.

    ini.SetUnicode();
    ini.LoadFile(Globals::configFileName.c_str());

    HANDLE hProcess = OpenGenshinProcess();
    if (hProcess == NULL)
    {
        std::cout << "Failed to open GenshinImpact process." << std::endl;
        return 1;
    }

    std::filesystem::current_path(path);
    ini.SaveFile(Globals::configFileName.c_str());

    std::string filename = (argc == 2 ? argv[1] : "CLibrary.dll");
    std::filesystem::path currentDllPath = std::filesystem::current_path() / filename;
    std::filesystem::path tempDllPath = std::filesystem::temp_directory_path() / filename;

    std::filesystem::copy(currentDllPath, tempDllPath, std::filesystem::copy_options::update_existing);
    InjectDLL(hProcess, tempDllPath.string());

    CloseHandle(hProcess);
}

HANDLE OpenGenshinProcess() 
{
    STARTUPINFOA startInfo{};
    PROCESS_INFORMATION processInformation{};

    auto filePath = GetOrSelectPath(ini, "Inject", "GenshinPath", "genshin path", "Executable\0*.exe\0");
    if (filePath.empty())
        return NULL;

    BOOL result = CreateProcessA(filePath.c_str(),
        nullptr, 0, 0, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &startInfo, &processInformation);
    if (result == FALSE) 
    {
        LogLastError("Failed to create game process.");
        LOG_ERROR("If you have problem with GenshinImpact.exe path. You can change it manually in %s.", Globals::configFileName.c_str());
        return NULL;
    }

    ini.SaveFile(Globals::configFileName.c_str());

    ResumeThread(processInformation.hThread);

    return processInformation.hProcess;
}
