#include "pch-il2cpp.h"
#include "Hotkey.h"

#include <Windows.h>
#include <string>

Hotkey::Hotkey() : Hotkey(0, 0) {}

Hotkey::Hotkey(short mKey, short aKey) : mKey(mKey), aKey(aKey) {}

Hotkey::operator std::string() const {
    auto mKeyName = GetKeyName(mKey);
    auto cKeyName = GetKeyName(aKey);
    if (cKeyName.size() > 0 && mKeyName.size() > 0) {
        return cKeyName + " + " + mKeyName;
    }
    else if (cKeyName.size() > 0) {
        return cKeyName;
    }
    else if (mKeyName.size() > 0) {
        return mKeyName;
    }
    return std::string();
}

bool Hotkey::IsPressed() {
    if (IsEmpty())
        return false;

    return (mKey == 0 || GetKeyState(mKey) & 0x8000) &&
        (aKey == 0 || GetKeyState(aKey) & 0x8000);
}

bool Hotkey::IsPressed(short keyDown)
{
    if (keyDown != mKey && keyDown != aKey)
        return false;

    auto checkKey = keyDown == mKey ? aKey : mKey;

    return (checkKey == 0 || GetKeyState(checkKey) & 0x8000);
}

bool Hotkey::IsEmpty() {
    return mKey == 0 && aKey == 0;
}

short Hotkey::GetAKey() {
    return aKey;
}

short Hotkey::GetMKey() {
    return mKey;
}

Hotkey Hotkey::GetPressedHotkey()
{
    short mKey = 0;
    short cKey = 0;
    for (short k = 0x10; k < 0x15; k++) {
        if (GetKeyState(k) & 0x8000)
        {
            cKey = k;
            break;
        }
    }

    static int mkeyRanges[5][2] = {
        { 0x20, 0x20 },
        { 0x22, 0x2E },
        { 0x30, 0x39 },
        { 0x41, 0x5A },
        { 0x60, 0x87 }
    };

    for (short r = 0; r < 5; r++) {
        auto range = mkeyRanges[r];
        for (short k = range[0]; k < range[1] + 1; k++) {
            if (GetKeyState(k) & 0x8000)
            {
                mKey = k;
                break;
            }
        }
        if (mKey > 0)
            break;
    }

    return Hotkey(mKey, cKey);
}

std::string Hotkey::GetKeyName(int keyId) {
    CHAR name[128];
    UINT scanCode = MapVirtualKeyA(keyId, MAPVK_VK_TO_VSC);
    LONG lParamValue = (scanCode << 16);
    int result = GetKeyNameTextA(lParamValue, name, 128);
    if (result > 0)
    {
        return std::string(name);
    }
    return std::string();
}
