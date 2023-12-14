#include "FollowObject.h"

FollowObject::FollowObject()
{
}

void FollowObject::SetParams(cMesh* mesh, cMesh* target, glm::vec3 offset, float minSpeed, float maxSpeed)
{
	this->mesh = mesh;
	this->target = target;
	this->minSpeed = minSpeed;
	this->maxSpeed = maxSpeed;
}

bool FollowObject::Execute(double deltaTime)
{
	glm::vec3 position = target->drawPosition + offset;
	//mesh->drawPosition = position;
	
	mesh->drawPosition = lerp(mesh->drawPosition, position, (float)deltaTime * minSpeed);

	return false;
}

glm::vec3 FollowObject::lerp(glm::vec3 x, glm::vec3 y, float t)
{
	return x * (1.f - t) + y * t;

}
