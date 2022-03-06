#pragma once

#include <Windows.h>
#include <string>

#include "Logger.h"

#define LogLastError(msg) LOG_ERROR("%s. Error: %s", msg, GetLastErrorAsString().c_str())

std::string GetLastErrorAsString();
bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size);