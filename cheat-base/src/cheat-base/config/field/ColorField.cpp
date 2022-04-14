#include <pch.h>
#include "ColorField.h"



void config::field::ColorField::Read(std::istream& io)
{
	ImU32 color;
	io >> color;
	*valuePtr() = ImColor(color);
}

void config::field::ColorField::Write(std::ostream& io)
{
	io << static_cast<ImU32>(base::value());
}
