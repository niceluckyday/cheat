#include "pch-il2cpp.h"
#include "HotkeyField.h"

#include <gcclib/Logger.h>
void config::field::HotkeyField::Write(std::ostream& io)
{
	io << value().GetMKey() << " " << value().GetAKey();
}

void config::field::HotkeyField::Read(std::istream& io)
{
	short mKey = 0;
	short aKey = 0;
	io >> mKey >> aKey;
	*valuePtr() = Hotkey(mKey, aKey);
}
