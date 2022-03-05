#pragma once
#include "IGUIModule.h"

class DebugModule :
	public IGUIModule
{
	void Draw() override;

	std::string GetName() override;
};

