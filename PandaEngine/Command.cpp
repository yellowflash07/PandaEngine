#include "Command.h"

Command::Command()
{
}

Command::Command(std::string name)
{
	this->name = name;
}


bool Command::Execute(double deltaTime)
{
	return false;
}

int Command::UniqueId()
{
	Random random;
	return random.GetRandomInt(0, 100000);
}

std::string Command::Name()
{
	return name;
}

void Command::SetName(std::string name)
{
	this->name = name;
}
