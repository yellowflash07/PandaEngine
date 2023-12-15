#include "LocationTrigger.h"

LocationTrigger::LocationTrigger()
{
}

bool LocationTrigger::Execute(double deltaTime)
{
	return false;
}

void LocationTrigger::SetParams(Engine engine, cMesh* meshToTrigger, glm::vec3 location, float size)
{
	cMesh* aabbMesh = 
}

