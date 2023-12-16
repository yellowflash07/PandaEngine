#include "cLuaCommandConfig.h"
 
#include <sol/state.hpp>
#include <sol/table.hpp>

void cLuaCommandConfig::LoadParams(std::string commandName, std::string meshName, std::string targetName, float speed)
{

	sol::state lua;

	lua.script_file(filename);
	sol::table myFunctions = lua[tableGroup];

	for(auto& entry : myFunctions)
	{
	sol::table commandGroups =  entry.second;


		 


			if(commandName == "MoveTo")
			{
				MoveTo* moveTo = (MoveTo*)commandFactory.CreateCommand("MoveTo","friendlyName");
				cMesh* mesh = engine.meshManager->FindMeshByFriendlyName("sp");
				cMesh* to = engine.meshManager->FindMeshByFriendlyName("mesh");
				
				moveTo->SetParams(mesh, to, speed);
				scene1.AddSerialCommand(moveTo);
			}

			 
	}
}
