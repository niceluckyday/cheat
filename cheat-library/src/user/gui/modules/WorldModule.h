#pragma once
#include "IGUIModule.h"

class WorldModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};



