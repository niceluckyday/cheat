#pragma once
#include <gui/IGUIModule.h>

class WorldModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};



