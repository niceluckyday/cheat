#pragma once
#include "IGUIModule.h"

class PlayerModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};



