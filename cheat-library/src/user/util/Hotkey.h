#pragma once

#include <string>

class Hotkey {
public:

    Hotkey();
    Hotkey(short mKey, short aKey);

    bool IsPressed();
    bool IsPressed(short keyDown);

    bool IsEmpty();
    short GetAKey();
    short GetMKey();

    friend bool operator== (const Hotkey& c1, const Hotkey& c2) {
        return c1.mKey == c2.mKey && c1.aKey == c2.aKey;
    }

    friend bool operator!= (const Hotkey & c1, const Hotkey & c2){
        return !(c1 == c2);
    }

    operator std::string() const;

    static Hotkey GetPressedHotkey();

private:
    // Main key can be 0-9, A-Z, Any Numpad, F1-F27, Space
    short mKey;

    // Additional key can be Shift, CapsLock, Ctrl, Alt
    short aKey;

    static std::string GetKeyName(int keyId);
};