#pragma once
#include "IGUIModule.h"

class SettingsModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};

