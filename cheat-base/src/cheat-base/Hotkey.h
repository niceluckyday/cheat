#pragma once

#include <string>
#include <unordered_set>

class Hotkey 
{
public:

    Hotkey();
    Hotkey(short key);
    Hotkey(std::vector<short> keys);

    bool IsPressed() const;
    bool IsPressed(short keyDown) const;
    bool IsReleased() const;

    bool IsEmpty() const;

    std::vector<short> GetKeys();

    friend bool operator== (const Hotkey& c1, const Hotkey& c2) {
        return c1.keys == c2.keys;
    }

    friend bool operator!= (const Hotkey & c1, const Hotkey & c2){
        return !(c1 == c2);
    }

    friend bool operator-(const Hotkey& c1, const Hotkey& c2)
    {
		for (short key : c1.keys)
		{
            if (c2.keys.count(key) == 0)
                return true;
		}
        return false;
    }

    operator std::string() const;

    static Hotkey GetPressedHotkey();

private:
    std::unordered_set<short> keys;
};