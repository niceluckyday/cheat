#pragma once
#include "IGUIModule.h"

class DebugModule :
	public IGUIModule
{
public:
	void Draw() override;

	std::string GetName() override;
};

