#pragma once
#include <string>

class IGUIModule
{
public:
	virtual ~IGUIModule() = default;
	virtual void Draw() = 0;

	virtual std::string GetName() = 0;
	virtual bool CanBeUnpinned() = 0;
};
