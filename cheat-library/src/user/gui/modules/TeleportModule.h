#pragma once
#include "IGUIModule.h"

class TeleportModule :
    public IGUIModule
{
	void Draw() override;

	std::string GetName() override;

	virtual void DrawInfo() override;
	virtual bool HasInfoToDraw() override;
};

