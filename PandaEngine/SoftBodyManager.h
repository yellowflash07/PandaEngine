#pragma once

#include <vector>
#include "SoftBody.h"
#include "MeshManager.h"

class SoftBodyManager
{

public:
	SoftBodyManager(MeshManager* meshManager);
	~SoftBodyManager();

	void Update(double deltaTime);
	SoftBody* CreateSoftBody(std::string friendlyName);

private:
	std::vector<SoftBody*> vecSoftBodies;
	MeshManager* meshManager;
};

