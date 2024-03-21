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
	toSet = true;
}

void MoveTo::SetParams(cMesh* mesh, cMesh* target, float speed)
{
	this->mesh = mesh;
	this->target = target;
	this->speed = speed;
	targetSet = true;
}

bool MoveTo::Execute(double deltaTime)
{
	currTime += (float)deltaTime;

	if (currTime < delay)
	{
		return false;
	}

	if (toSet)
	{
		float distance = glm::distance(mesh->transform.drawPosition, to);
		if (distance > 0.1f)
		{
			glm::vec3 direction = glm::normalize(to - mesh->transform.drawPosition);
			mesh->transform.drawPosition += direction * speed * (float)deltaTime;
			return false;
		}
	}
	if (targetSet)
	{
		float distance = glm::distance(mesh->transform.drawPosition, target->transform.drawPosition);
		if (distance > 0.1f)
		{
			glm::vec3 direction = glm::normalize(target->transform.drawPosition - mesh->transform.drawPosition);
			mesh->transform.drawPosition += direction * speed * (float)deltaTime;
			return false;
		}
	}
	
	if (OnComplete)
	{
		OnComplete();
	}
	//mesh->drawPosition = to;
	return true;
}


