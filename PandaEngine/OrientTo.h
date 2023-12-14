#pragma once

#include "Command.h"
#include "cMesh.h"

class OrientTo
	: public Command
{
public:
	OrientTo();
	virtual ~OrientTo() {};

	void SetParams(cMesh* mesh, glm::vec3 to, float speed, float time);
	virtual bool Execute(double deltaTime) override;

private:
	cMesh* mesh;
	glm::vec3 to;
	float speed;
	float time;
	float currTime;
};

