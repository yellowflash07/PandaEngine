#pragma once
#include <string>

#include "cMesh.h"
#include "CommandFactory.h"
#include "CommandGroup.h"
#include "Engine.h"
#include "MoveTo.h"
class cLuaCommandConfig
{

public:
	cLuaCommandConfig();
	~cLuaCommandConfig();

	void LoadParams(std::string commandName, std::string meshName, std::string targetName, float speed);
	char tableGroup;

private:
	const char* filename = "MoveTo.lua";
	CommandFactory commandFactory;
	CommandGroup scene1;
	float speed;
	Engine engine;

};

