#pragma once
#include "cLightManager.h"
#include <sstream> //"string stream"
#include <imgui.h>
#include "ImGuizmo.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../Basic Shader Manager/cShaderManager.h"

extern Camera* camera;

cLight::cLight()
{
	this->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White light

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	this->atten = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);	
	// Spot, directional lights
	// (Default is stright down)
	this->direction = glm::vec4(-1.0, -1.0, -1.0, 1);
	// x = lightType, y = inner angle, z = outer angle, w = TBD
	// type = 0 => point light
	this->param1 = glm::vec4(2.0f, 0.0f, 0.0f, 1.0f);
					// 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	// x = 0 for off, 1 for on
	this->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	this->position_UL = -1;
	this->diffuse_UL = -1;
	this->specular_UL = -1;
	this->atten_UL = -1;
	this->direction_UL = -1;
	this->param1_UL = -1;
	this->param2_UL = -1;

	uniformLocationIsSet = false;
}

	// And the uniforms:
void cLight::TurnOn(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 1.0f;		// Turn on 
	return;
}

void cLight::TurnOff(void)
{
	// x = 0 for off, 1 for on
	this->param2.x = 0.0f;		// Turn off 
	return;
}

void cLight::SetUniformLocations(GLuint shaderID, int lightIndex)
{
	this->shaderID = shaderID;
	this->index = lightIndex;
	std::string lightPositon = "theLights[" + std::to_string(lightIndex) + "]" + ".position";
	std::string lightdiffuse = "theLights[" + std::to_string(lightIndex) + "]" + ".diffuse";
	std::string lightspecular = "theLights[" + std::to_string(lightIndex) + "]" + ".specular";
	std::string lightatten = "theLights[" + std::to_string(lightIndex) + "]" + ".atten";
	std::string lightdirection = "theLights[" + std::to_string(lightIndex) + "]" + ".direction";
	std::string lightparam1 = "theLights[" + std::to_string(lightIndex) + "]" + ".param1";
	std::string lightparam2 = "theLights[" + std::to_string(lightIndex) + "]" + ".param2";
	//vec4 position
	position_UL = glGetUniformLocation(shaderID, lightPositon.c_str());
	//        vec4 diffuse;	// Colour of the light (used for diffuse)
	diffuse_UL = glGetUniformLocation(shaderID, lightdiffuse.c_str());
	//        vec4 specular;	// rgb = highlight colour, w = power
	specular_UL = glGetUniformLocation(shaderID, lightspecular.c_str());
	//        vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	atten_UL = glGetUniformLocation(shaderID, lightatten.c_str());
	//        vec4 direction;	// Spot, directional lights
	direction_UL = glGetUniformLocation(shaderID, lightdirection.c_str());
	//        vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	param1_UL = glGetUniformLocation(shaderID, lightparam1.c_str());
	//        vec4 param2;	// x = 0 for off, 1 for on
	param2_UL = glGetUniformLocation(shaderID, lightparam2.c_str());
}

void cLight::UpdateLight(TransformComponent* transform)
{
	this->position = glm::vec4(transform->drawPosition, 1.0f);

	glUniform4f(this->position_UL,
		position.x,
		position.y,
		position.z,
		1.0f);

	glUniform4f(this->diffuse_UL,
		this->diffuse.x,
		this->diffuse.y,
		this->diffuse.z,
		this->diffuse.w);

	glUniform4f(this->specular_UL,
		this->specular.x,
		this->specular.y,
		this->specular.z,
		this->specular.w);

	glUniform4f(this->atten_UL,
		this->atten.x,
		this->atten.y,
		this->atten.z,
		this->atten.w);

	glUniform4f(this->direction_UL,
		this->direction.x,
		this->direction.y,
		this->direction.z,
		this->direction.w);

	glUniform4f(this->param1_UL,
		this->param1.x,
		this->param1.y,
		this->param1.z,
		this->param1.w);

	glUniform4f(this->param2_UL,
		this->param2.x,
		this->param2.y,
		this->param2.z,
		this->param2.w);

}

void cLight::Render()
{
	ImGui::BeginChild(("Light" + std::to_string(index)).c_str(), ImVec2(0, 200));

	ImGui::SeparatorText("Light Component");

	//ImGui::Text("On?"); ImGui::SetNextItemWidth(40);
	//ImGui::InputFloat("On", &param2.x);

	if (ImGui::Checkbox("On", &bIsOn))
	{
		if (bIsOn)
		{
			TurnOn();
		}
		else
		{
			TurnOff();
		}
	}

	ImGui::SetNextItemWidth(100);
	int type = param1.x;
	ImGui::Combo("Light Type", &type, "Point Light\0Spot Light\0Directional\0\0"); ImGui::SetNextItemWidth(40);
	param1.x = type;
	ImGui::InputFloat("inner angle", &param1.y); ImGui::SameLine();
	ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("outer angle", &param1.z);

	ImGui::Text("Position"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xP", &position.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yP", &position.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zP", &position.z);
	position.w = 1;

	ImGui::Text("Diffuse"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xD", &diffuse.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yD", &diffuse.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zD", &diffuse.z);
	diffuse.w = 1;

	ImGui::Text("Specular"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xS", &specular.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yS", &specular.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zS", &specular.z);
	specular.w = 1;

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	ImGui::Text("Attenuation"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("constant", &atten.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("linear", &atten.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("quadratic", &atten.z); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("DistanceCutOff", &atten.w);

	ImGui::Text("Direction"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xDi", &direction.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yDi", &direction.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zDi", &direction.z);

	ImGui::Separator();

	ImGui::EndChild();
}


void cLightManager::SetUniformLocations(GLuint shaderID)
{

	for (size_t i = 0; i < NUMBER_OF_LIGHTS_IM_USING; i++)
	{
		std::string lightPositon = "theLights[" + std::to_string(i) + "]" + ".position";
		std::string lightdiffuse = "theLights[" + std::to_string(i) + "]" + ".diffuse";
		std::string lightspecular = "theLights[" + std::to_string(i) + "]" + ".specular";
		std::string lightatten = "theLights[" + std::to_string(i) + "]" + ".atten";
		std::string lightdirection = "theLights[" + std::to_string(i) + "]" + ".direction";
		std::string lightparam1 = "theLights[" + std::to_string(i) + "]" + ".param1";
		std::string lightparam2 = "theLights[" + std::to_string(i) + "]" + ".param2";
		//vec4 position
		this->theLights[i]->position_UL = glGetUniformLocation(shaderID, lightPositon.c_str());
		//        vec4 diffuse;	// Colour of the light (used for diffuse)
		this->theLights[i]->diffuse_UL = glGetUniformLocation(shaderID, lightdiffuse.c_str());
		//        vec4 specular;	// rgb = highlight colour, w = power
		this->theLights[i]->specular_UL = glGetUniformLocation(shaderID, lightspecular.c_str());
		//        vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
		this->theLights[i]->atten_UL = glGetUniformLocation(shaderID, lightatten.c_str());
		//        vec4 direction;	// Spot, directional lights
		this->theLights[i]->direction_UL = glGetUniformLocation(shaderID, lightdirection.c_str());
		//        vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
		this->theLights[i]->param1_UL = glGetUniformLocation(shaderID, lightparam1.c_str());
		//        vec4 param2;	// x = 0 for off, 1 for on
		this->theLights[i]->param2_UL = glGetUniformLocation(shaderID, lightparam2.c_str());
	}

	return;
}

// This is called every frame
void cLightManager::UpdateUniformValues(GLuint shaderID)
{
	for ( unsigned int index = 0; index != cLightManager::NUMBER_OF_LIGHTS_IM_USING; index++ )
	{
		cLight* pCurrentLight = theLights[index];
		std::string label = "Light [" + std::to_string(index) + "]";
		ImGui::Begin("Lights");
		if (ImGui::Button(label.c_str(), ImVec2(100, 25)))
		{
			selectedLight = pCurrentLight;
		}
		ImGui::End();
	}// for ( unsigned int index...

	UpdateLights(shaderID);

	DrawBox();
	return;
}

void cLightManager::UpdateLight(cLight* light, TransformComponent* transform)
{

	glUniform4f(light->position_UL,
				transform->drawPosition.x,
				transform->drawPosition.y,
				transform->drawPosition.z,
				1.0f);

	glUniform4f(light->diffuse_UL,
				light->diffuse.x,
				light->diffuse.y,
				light->diffuse.z,
				light->diffuse.w);

	glUniform4f(light->specular_UL,
				light->specular.x,
				light->specular.y,
				light->specular.z,
				light->specular.w);

	glUniform4f(light->atten_UL,
				light->atten.x,
				light->atten.y,
				light->atten.z,
				light->atten.w);

	glUniform4f(light->direction_UL,
				light->direction.x,
				light->direction.y,
				light->direction.z,
				light->direction.w);

	glUniform4f(light->param1_UL,
				light->param1.x,
				light->param1.y,
				light->param1.z,
				light->param1.w);

	glUniform4f(light->param2_UL,
				light->param2.x,
				light->param2.y,
				light->param2.z,
				light->param2.w);
}

void cLightManager::UpdateLights(GLuint shaderID)
{
	for (unsigned int index = 0; index != cLightManager::NUMBER_OF_LIGHTS_IM_USING; index++)
	{
		cLight* pCurrentLight = theLights[index];
		glUniform4f(pCurrentLight->position_UL,
			pCurrentLight->position.x,
			pCurrentLight->position.y,
			pCurrentLight->position.z,
			pCurrentLight->position.w);

		glUniform4f(theLights[index]->diffuse_UL,
			theLights[index]->diffuse.x,
			theLights[index]->diffuse.y,
			theLights[index]->diffuse.z,
			theLights[index]->diffuse.w);

		glUniform4f(theLights[index]->specular_UL,
			theLights[index]->specular.x,
			theLights[index]->specular.y,
			theLights[index]->specular.z,
			theLights[index]->specular.w);

		glUniform4f(theLights[index]->atten_UL,
			theLights[index]->atten.x,
			theLights[index]->atten.y,
			theLights[index]->atten.z,
			theLights[index]->atten.w);

		glUniform4f(theLights[index]->direction_UL,
			theLights[index]->direction.x,
			theLights[index]->direction.y,
			theLights[index]->direction.z,
			theLights[index]->direction.w);

		glUniform4f(theLights[index]->param1_UL,
			theLights[index]->param1.x,
			theLights[index]->param1.y,
			theLights[index]->param1.z,
			theLights[index]->param1.w);

		glUniform4f(theLights[index]->param2_UL,
			theLights[index]->param2.x,
			theLights[index]->param2.y,
			theLights[index]->param2.z,
			theLights[index]->param2.w);
	}
}

void cLightManager::DrawBox()
{
	if (selectedLight == nullptr) return;
//	std::string boxName = "Transform " + selectedMesh->friendlyName;
	ImGui::Begin("Light Value");

	glm::mat4 matModel = glm::mat4(1.0f);
	matModel = glm::translate(matModel, glm::vec3(selectedLight->position));
	matModel = glm::mat4(glm::quat(selectedLight->direction)) * matModel;

	static ImGuizmo::OPERATION gizmoOperation(ImGuizmo::TRANSLATE);

	bool isTranslateRadio = false;
	ImGui::NewLine();
	if (ImGui::RadioButton("Translate", &isTranslateRadio))
	{
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	bool isRotateRadio = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", &isRotateRadio))
	{
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}

	ImGuizmo::Manipulate(glm::value_ptr(camera->matView),
		glm::value_ptr(camera->matProjection),
		gizmoOperation,
		ImGuizmo::MODE::LOCAL,
		glm::value_ptr(matModel));

	/*if (ImGuizmo::IsUsing())
	{
		glm::vec3 position, scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matModel, scale, rotation, position, skew, perspective);
		selectedLight->position = glm::vec4(position,0);
		selectedLight->direction = glm::vec4(rotation.x, rotation.y, rotation.z, 1);
	}*/

	ImGui::Text("On?"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("On", &selectedLight->param2.x);

	ImGui::Text("Type"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("Type", &selectedLight->param1.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("inner angle", &selectedLight->param1.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("outer angle", &selectedLight->param1.z);

	ImGui::Text("Position"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xL", &selectedLight->position.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yL", &selectedLight->position.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zL", &selectedLight->position.z);
	selectedLight->position.w = 1;

	ImGui::Text("Diffuse"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xD", &selectedLight->diffuse.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yD", &selectedLight->diffuse.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zD", &selectedLight->diffuse.z);
	selectedLight->diffuse.w = 1;

	ImGui::Text("Specular"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xS", &selectedLight->specular.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yS", &selectedLight->specular.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zS", &selectedLight->specular.z);
	selectedLight->specular.w = 1;

	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	ImGui::Text("Attenuation"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("constant", &selectedLight->atten.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("linear", &selectedLight->atten.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("quadratic", &selectedLight->atten.z); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("DistanceCutOff", &selectedLight->atten.w); 

	ImGui::Text("Direction"); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("xDi", &selectedLight->direction.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("yDi", &selectedLight->direction.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("zDi", &selectedLight->direction.z);
	//ImGui::InputFloat("xDi", &selectedLight->atten.z); ImGui::SameLine(); ImGui::SetNextItemWidth(40);

	ImGui::End();
}

void cLightManager::LoadLights()
{
	/*SceneSaver saver;
	std::vector<cLight> loadedLights = saver.LoadLights();
	for (size_t i = 0; i < loadedLights.size(); i++)
	{
		theLights[i] = &loadedLights[i];
	}*/
}


cLightManager::cLightManager()
{
	for (size_t i = 0; i < NUMBER_OF_LIGHTS_IM_USING; i++)
	{
		theLights[i] = new cLight();
	}
}