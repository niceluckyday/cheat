#pragma once
#include <string>

class IGUIModule
{
public:
	virtual void Draw() = 0;
	virtual std::string GetName() = 0;

	virtual void DrawInfo() { };
	virtual bool HasInfoToDraw() { return false; }
};
