#include "OrientTo.h"
#include <cmath>
#include <string>
#include <glm/gtx/string_cast.hpp>

OrientTo::OrientTo()
{
}

void OrientTo::SetParams(cMesh* mesh, glm::vec3 to, float speed, float time)
{
	this->mesh = mesh;
	this->to = to;
	this->speed = speed;
	this->time = time;
}

bool OrientTo::Execute(double deltaTime)
{
	glm::vec3 direction = glm::normalize(to - mesh->drawPosition);
//	glm::quat targetOrientation = glm::normalize(glm::quatLookAt(direction, glm::vec3(0,1,0)));
	glm::quat targetOrientation = glm::quatLookAt(direction, glm::vec3(0,1,0));

	bool isDone = false;
	glm::quat resultQuat = glm::quat(0,0,0,0);

	if (speed > 0)
	{
		float interpolationFactor = speed * currTime;
		interpolationFactor = glm::clamp(interpolationFactor, 0.0f, 1.0f);
		resultQuat = glm::slerp(mesh->get_qOrientation(), targetOrientation, speed * (float)deltaTime);
		mesh->setRotationFromQuat(glm::normalize(resultQuat));

		// Check if the interpolation is complete based on speed
		if (glm::length(targetOrientation - resultQuat) < 0.001f)
		{
			isDone = true;  // Return false to indicate completion
			//return true;  // Return false to indicate completion
		}

	}
	
	if (time > 0)
	{
		resultQuat = glm::slerp(glm::normalize(mesh->get_qOrientation()), targetOrientation, (float)deltaTime/ time);
		mesh->setRotationFromQuat(glm::normalize(resultQuat));
		
		if (glm::length(targetOrientation - resultQuat) < 0.001f)
		{
			isDone = true;  // Return false to indicate completion
			//return true;  // Return false to indicate completion
		}
	}

	if (!isDone)
	{
		std::cout << "Result Speed: " << speed * (float)deltaTime << std::endl;
		//std::cout << "Target Quaternion: " << glm::to_string(targetOrientation) << std::endl;
		//std::cout << "Result Quaternion: " << glm::to_string(resultQuat) << std::endl;
	}

	return isDone;
}
