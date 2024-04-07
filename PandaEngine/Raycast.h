#pragma once

#include "PhysXManager.h"
#include "PhysXBody.h"

struct HitResult
{
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
	float distance;
	PhysXBody* hitActor;
};

class Raycast
{
public:
	Raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance);
	~Raycast();

	bool RaycastHit(HitResult& hitResult);

private:
	glm::vec3 origin;
	glm::vec3 direction;
	float maxDistance;
	
	PxScene* gScene = NULL;
};

