#pragma once
#include "IGUIModule.h"

class HotkeysModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};



