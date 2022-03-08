#pragma once
#include "IGUIModule.h"

class DebugModule :
	public IGUIModule
{
public:
	DebugModule();

	void Draw() override;

	std::string GetName() override;
};

