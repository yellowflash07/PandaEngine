#pragma once
#include "Command.h"
#include "MoveTo.h"
#include "OrientTo.h"
#include "FollowObject.h"
#include "FollowCurve.h"
class CommandFactory
{
public:
	CommandFactory();
	~CommandFactory();

	iCommand* CreateCommand(std::string name);
};
