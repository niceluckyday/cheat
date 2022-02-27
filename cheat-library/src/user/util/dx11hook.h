#pragma once
#include <Windows.h>

#define BType_SendImGUI 1
#define BType_Blocked 2

typedef void (*RenderCallback)();
typedef void (*OnKeyUpCallback)(WPARAM key, short& ioFlag);

void createOverlay(RenderCallback renderFunc, OnKeyUpCallback onKeyUpFunc, LPBYTE fontData, DWORD fontSize);