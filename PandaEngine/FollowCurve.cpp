#include "FollowCurve.h"
#include <glm/gtx/spline.hpp>
FollowCurve::FollowCurve()
{
}

void FollowCurve::SetParams(cMesh* mesh, std::vector<glm::vec3> curvePoints, float speed, float time)
{
	this->mesh = mesh;
	this->curvePoints = curvePoints;
	this->speed = speed;
	this->time = time;
}

bool FollowCurve::Execute(double deltaTime)
{
	currTime += (float)deltaTime;
	float t = currTime / time;
	if (currTime > 1.f)
	{
		currTime = 0.0f;
		pointIndex += 1;
	}

	if (pointIndex < curvePoints.size() - 3)
	{
		mesh->drawPosition = PointOnCurve(curvePoints[pointIndex],
							curvePoints[pointIndex + 1],
							curvePoints[pointIndex + 2],
							curvePoints[pointIndex + 3], (float)currTime);

		glm::vec3 direction = glm::normalize(curvePoints[pointIndex + 2] - mesh->drawPosition);
		glm::quat targetOrientation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		mesh->setRotationFromQuat(glm::normalize(targetOrientation));

		return false;
	}
	
	return true;
}

//from https://glm.g-truc.net/0.9.4/api/a00203.html
glm::vec3 FollowCurve::PointOnCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	return glm::catmullRom(p0,p1,p2,p3,t);
}
