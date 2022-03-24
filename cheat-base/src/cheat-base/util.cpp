#include <pch.h>
#include <framework.h>
#include "util.h"

#include <Windows.h>
#include <commdlg.h>
#include <shtypes.h>
#include <shobjidl_core.h>
#include <shlobj_core.h>

#include <sstream>
#include <string>
#include <iomanip>
#include <codecvt>
#include <filesystem>

namespace util
{

    std::string GetLastErrorAsString()
    {
        //Get the error message ID, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
        {
            return std::string(); //No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        //Copy the error message into a std::string.
        std::string message(messageBuffer, size);

        //Free the Win32's string's buffer.
        LocalFree(messageBuffer);

        return message;
    }

    std::string to_hex_string(uint8_t* barray, int length)
    {
        if (barray == nullptr || length == 0)
            return std::string();

        std::stringstream stream;
        for (size_t i = 0; i < length; i++)
            stream << std::setfill('0') << std::setw(2) << std::hex << (int)barray[i];

        return stream.str();
    }

    std::optional<std::string> SelectDirectory(const char* title)
    {
        auto currPath = std::filesystem::current_path();

        if (!SUCCEEDED(CoInitialize(nullptr)))
            return {};

        IFileDialog* pfd;
        if (!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
            return {};

        const size_t titleSize = strlen(title) + 1;
        wchar_t* wcTitle = new wchar_t[titleSize];
        mbstowcs(wcTitle, title, titleSize);

        DWORD dwOptions;
        IShellItem* psi;
        if (!SUCCEEDED(pfd->GetOptions(&dwOptions)) ||
            !SUCCEEDED(pfd->SetOptions(dwOptions | FOS_PICKFOLDERS)) ||
            !SUCCEEDED(pfd->SetTitle(wcTitle)) ||
            !SUCCEEDED(pfd->Show(NULL)) ||
            !SUCCEEDED(pfd->GetResult(&psi)))
        {
            pfd->Release();
            return {};
        }

        WCHAR* folderName;
        if (!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &folderName)))
        {
            pfd->Release();
            psi->Release();
            return {};
        }

        pfd->Release();
        psi->Release();

        std::filesystem::current_path(currPath);

        std::u16string u16(reinterpret_cast<const char16_t*>(folderName));
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
    }

    std::optional<std::string> SelectFile(const char* filter, const char* title)
    {
        auto currPath = std::filesystem::current_path();

        // common dialog box structure, setting all fields to 0 is important
        OPENFILENAME ofn = { 0 };
        TCHAR szFile[260] = { 0 };

        // Initialize remaining fields of OPENFILENAME structure
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.lpstrTitle = title;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        std::optional<std::string> result = {};
        if (GetOpenFileName(&ofn) == TRUE)
            result = std::string(szFile);

        std::filesystem::current_path(currPath);
        return result;
    }

    std::optional<std::string> GetOrSelectPath(CSimpleIni& ini, const char* section, const char* name, const char* friendName, const char* filter)
    {
        auto savedPath = ini.GetValue(section, name);
        if (savedPath != nullptr)
            return std::string(savedPath);

        LOG_DEBUG("%s path not found. Please point to it manually.", friendName);

        auto titleStr = string_format("Select %s", friendName);
        auto selectedPath = filter == nullptr ? SelectDirectory(titleStr.c_str()) : SelectFile(filter, titleStr.c_str());
        if (!selectedPath)
            return {};

        ini.SetValue(section, name, selectedPath->c_str());
        return selectedPath;
    }

	bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size)
	{
		HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
		if (hResource) {
			HGLOBAL hGlob = LoadResource(hInstance, hResource);
			if (hGlob) {
				size = SizeofResource(hInstance, hResource);
				pDest = static_cast<LPBYTE>(LockResource(hGlob));
				if (size > 0 && pDest)
					return true;
			}
		}
		return false;
	}

	int64_t GetCurrentTimeMillisec()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}