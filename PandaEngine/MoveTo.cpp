#include "MoveTo.h"

MoveTo::MoveTo()
{
}

MoveTo::MoveTo(cMesh* mesh, glm::vec3 to, float speed)
{
	this->mesh = mesh;
	this->to = to;
	this->speed = speed;
}

void MoveTo::SetParams(cMesh* mesh, glm::vec3 to, float speed)
{
	this->mesh = mesh;
	this->to = to;
	this->speed = speed;
}

bool MoveTo::Execute(double deltaTime)
{
	float distance = glm::distance(mesh->drawPosition, to);
	if (distance > 0.1f)
	{
		glm::vec3 direction = glm::normalize(to - mesh->drawPosition);
		mesh->drawPosition += direction * speed * (float)deltaTime;
		return false;
	}

	mesh->drawPosition = to;
	return true;
}


