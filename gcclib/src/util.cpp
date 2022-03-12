#include <pch.h>
#include <framework.h>
#include <gcclib/util.h>

#include <Windows.h>
#include <commdlg.h>
#include <shtypes.h>
#include <shobjidl_core.h>
#include <shlobj_core.h>

#include <sstream>
#include <string>
#include <iomanip>


std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
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

std::string to_hex_string(uint8_t* barray, int length) {
    if (barray == nullptr || length == 0)
        return std::string();

    std::stringstream stream;
    for (size_t i = 0; i < length; i++)
    {
        stream << std::setfill('0') << std::setw(2) << std::hex << (int)barray[i];
    }
    return stream.str();
}

std::string* GetOpenDirectory()
{
    CoInitialize(nullptr);
	std::string* ret = nullptr;
	IFileDialog* pfd;
	if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
	{
		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
		{
			pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
		}
		if (SUCCEEDED(pfd->Show(NULL)))
		{
			IShellItem* psi;
			if (SUCCEEDED(pfd->GetResult(&psi)))
			{
				WCHAR* tmp;
				if (SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &tmp)))
				{
					char buf[256] = {};
                    char* c = buf;
                    size_t size = 0;
					while (*tmp && c - buf < 255)
					{
						*c = (char)*tmp;
						++c;
						++tmp;
                        ++size;
					}
                    ret = new std::string(size, '\0');
                    memcpy_s(ret->data(), size, buf, size);
				}
				psi->Release();
			}
		}
		pfd->Release();
	}
	return ret;
}

std::string* SelectFile(const char* filter) 
{
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };

    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return new std::string(szFile);
    }

    return nullptr;
}