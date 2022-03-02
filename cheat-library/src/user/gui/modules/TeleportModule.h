#pragma once
#include "IGUIModule.h"

class TeleportModule :
    public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
	bool CanBeUnpinned() override;
};

