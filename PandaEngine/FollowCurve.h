#pragma once
#include "Command.h"
#include "cMesh.h"
#include <vector>

class FollowCurve :
    public Command
{
    public:
	FollowCurve();
	virtual ~FollowCurve() {};

	void SetParams(cMesh* mesh, std::vector<glm::vec3> curvePoints, float speed, float time);
	virtual bool Execute(double deltaTime) override;

private:
	cMesh* mesh;
	std::vector<glm::vec3> curvePoints;
	float speed;
	float time;
	float currTime;
	int pointIndex;
	glm::vec3 PointOnCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);
};

