#include "Raycast.h"

extern std::map<PxActor*, PhysXBody*> gActorMap;

Raycast::Raycast(glm::vec3 origin, glm::vec3 direction, float maxDistance)
{
	this->origin = origin;
	this->direction = direction;
	this->maxDistance = maxDistance;
	this->gScene = PhysXManager::getInstance()->gScene;
}

Raycast::~Raycast()
{
}

bool Raycast::RaycastHit(HitResult& hitResult)
{
	PxRaycastBuffer hit;
	PxVec3 origin = PxVec3(this->origin.x, this->origin.y, this->origin.z);
	PxVec3 unitDir = PxVec3(this->direction.x, this->direction.y, this->direction.z);
	PxReal maxDistance = this->maxDistance;
	PxHitFlags hitFlags = PxHitFlag::eDEFAULT | PxHitFlag::eMESH_ANY | PxHitFlag::ePOSITION;
	PxQueryFilterData filterData;
	filterData.flags =  PxQueryFlag::eSTATIC;

	bool hashit =  gScene->raycast(origin, unitDir, maxDistance, hit, hitFlags, filterData);

	if (hit.hasBlock)
	{
		hitResult.hitPoint = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
		hitResult.hitNormal = glm::vec3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
		hitResult.distance = hit.block.distance;

		hitResult.hitActor = (PhysXBody*)hit.block.actor->userData;
		return true;
	}

	return false;
}
