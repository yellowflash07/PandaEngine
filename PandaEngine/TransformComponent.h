#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 drawPosition = glm::vec3(0);
	glm::vec3 eulerRotation = glm::vec3(0);
	glm::vec3 drawScale = glm::vec3(1);
	glm::quat m_qOrientation = glm::quat(eulerRotation);

	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
	{
		eulerRotation = newEulerAngleXYZ;
		this->m_qOrientation = glm::quat(eulerRotation);
	}

	void setRotationFromQuat(glm::quat newQuat)
	{
		eulerRotation = glm::eulerAngles(newQuat);
		this->m_qOrientation = newQuat;
	}

	void Rotate(glm::vec3 EulerAngleXYZ_Adjust)
	{
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
		this->m_qOrientation *= qChange;
	}

	glm::quat get_qOrientation(void)
	{
		return this->m_qOrientation;
	}

	glm::vec3 GetForwardVector(void)
	{
		glm::mat4 matRotation = glm::mat4(this->m_qOrientation);
		glm::vec4 forward4 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glm::vec4 newForward4 = matRotation * forward4;
		glm::vec3 newForward = newForward4;
		return newForward;
	}

	glm::vec3 GetRightVector(void)
	{
		glm::vec3 newRight = glm::cross(this->GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f));
		return newRight;
	}

	void setUniformDrawScale(float scale)
	{
		this->drawScale = glm::vec3(scale, scale, scale);
	}


	glm::mat4 GetTransform()
	{
		glm::mat4 matModel = glm::mat4(1.0f);

		// Translation
		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f), drawPosition);

		// Rotation matrix generation
		glm::mat4 matRotation = glm::mat4(get_qOrientation());

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f), drawScale);

		return matModel * matTranslate * matRotation * matScale;
	}

};