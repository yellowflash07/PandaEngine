#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "IEditorUI.h"
#include "Camera.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtx/matrix_decompose.hpp>

extern Camera* camera;

struct TransformComponent : public IEditorUI
{
	glm::vec3 drawPosition = glm::vec3(0);
	glm::vec3 eulerRotation = glm::vec3(0);
	glm::vec3 drawScale = glm::vec3(1);
	glm::quat m_qOrientation = glm::quat(eulerRotation);

	bool isDirty = false;

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
		glm::mat4 matRotation = glm::mat4(glm::quat(eulerRotation));

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f), drawScale);

		return matModel * matTranslate * matRotation * matScale;
	}

	void Render()
	{
		ImGui::BeginChild("Transform", ImVec2(0, 200));
		ImGui::Text("Transform");
		ImGui::Text("Position"); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("xP", &drawPosition.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("yP", &drawPosition.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("zP", &drawPosition.z);

		ImGui::Text("Rotation"); ImGui::SetNextItemWidth(40);
		glm::vec3 euler = eulerRotation;
		ImGui::InputFloat("xR", &euler.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("yR", &euler.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("zR", &euler.z);
		setRotationFromEuler(euler);

		ImGui::Text("Scale"); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("xS", &drawScale.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("yS", &drawScale.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
		ImGui::InputFloat("zS", &drawScale.z); ImGui::SetNextItemWidth(100);

		glm::mat4 meshTransform = GetTransform();

		static ImGuizmo::OPERATION gizmoOperation(ImGuizmo::OPERATION::UNIVERSAL);
		bool isTranslateRadio = false;

		if (ImGui::RadioButton("Translate", gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)) 
		{
			gizmoOperation = ImGuizmo::OPERATION::TRANSLATE; 
		} ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", gizmoOperation == ImGuizmo::OPERATION::ROTATE))
		{ 
			gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		} ImGui::SameLine();
		if (ImGui::RadioButton("Scale", gizmoOperation == ImGuizmo::OPERATION::SCALE))
		{ 
			gizmoOperation = ImGuizmo::OPERATION::SCALE;
		}

		ImGuizmo::Manipulate(glm::value_ptr(camera->matView),
			glm::value_ptr(camera->matProjection),
			gizmoOperation,
			ImGuizmo::MODE::LOCAL,
			glm::value_ptr(meshTransform));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 position, scale;
			glm::quat rotation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(meshTransform, scale, rotation, position, skew, perspective);
			drawPosition = position;
			drawScale = scale;
			setRotationFromQuat(rotation);
		}

		ImGui::Separator();
		ImGui::EndChild();
	}

};