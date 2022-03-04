#pragma once
#include <gui/IGUIModule.h>

class PlayerModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};



