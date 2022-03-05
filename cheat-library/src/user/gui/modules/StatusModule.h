#pragma once
#include "IGUIModule.h"

class StatusModule :
	public IGUIModule
{
public:
	void Draw() override;

	std::string GetName() override;
};



