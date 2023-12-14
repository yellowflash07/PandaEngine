#include "CommandFactory.h"

CommandFactory::CommandFactory()
{
}

CommandFactory::~CommandFactory()
{
}

iCommand* CommandFactory::CreateCommand(std::string name)
{
    iCommand* command = nullptr;

    if (name == "MoveTo")
    {
		command = new MoveTo();
	}

    if (name == "OrientTo")
    {
		command = new OrientTo();
	}

    if (name == "FollowObject")
    {
		command = new FollowObject();
	}

    if (name == "FollowCurve")
    {
        command = new FollowCurve();
    }

    return command;
}
