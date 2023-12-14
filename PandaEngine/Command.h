#pragma once

#include "iCommand.h"
#include "Random.h"
#include <iostream>
class Command :
	public iCommand
{
public:
	Command();
	Command(std::string name);
	virtual ~Command() {};

	virtual bool Execute(double deltaTime);
	virtual int UniqueId();
	virtual std::string Name();
	virtual void SetName(std::string name);

private:
	std::string name;
	
};

