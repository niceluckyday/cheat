#pragma once
#include <gui/IGUIModule.h>

class ActiveFeaturesModule :
	public IGUIModule
{
public:
	void Draw() override;

	std::string GetName() override;
};



