#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>
#include "PhysXManager.h"
#include "PhysXBody.h"
#include "Raycast.h"
#include <glm/gtx/string_cast.hpp>
#include <map>
#include "cPlayerAnimations.h"
extern Camera* camera;
int keyHit = 0;

#include "Scene.h"

std::map<int, bool> keyMap;

bool IsKeyPressed(int key)
{
	return keyMap[key];
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keyHit = key;
		keyMap[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		keyHit = 0;
		keyMap[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera->ProcessMouseMovement(xpos, ypos);
}



int main(void)
{
	Engine engine;
	if (!engine.Initialize())
	{
		return 1;
	}

	bool loaded = engine.meshManager->LoadCubeMap("space",
		"CubeMaps/TropicalSunnyDayLeft2048.bmp",
		"CubeMaps/TropicalSunnyDayRight2048.bmp",
		"CubeMaps/TropicalSunnyDayDown2048.bmp",
		"CubeMaps/TropicalSunnyDayUp2048.bmp",
		"CubeMaps/TropicalSunnyDayFront2048.bmp",
		"CubeMaps/TropicalSunnyDayBack2048.bmp",
		true);

	camera->SetPosition(glm::vec3(11.0f, 158.0f, 401.0f));

	engine.LoadSave("AnimationScene.json");

	float currTime = 0;
	float myTime = 0;

	camera->speed = 150.0f;

	Scene* scene = engine.GetCurrentScene();

	GameObject* gun = scene->GetGameObjectByName("Pistol");
	TransformComponent* gunTransformComponent = gun->GetComponent<TransformComponent>();

	//Vampire
	//GameObject* vampire = scene->GetGameObjectByName("Vampire");
	//AnimationSystem* vampireAnimationSystem = &vampire->AddComponent<AnimationSystem>();

	//Sphere
	//GameObject* sphere = scene->GetGameObjectByName("Sphere");
	//TransformComponent* sphereTransformComponent = sphere->GetComponent<TransformComponent>();

	cPlayerAnimations playerAnimations;
	playerAnimations.playerGameObject = scene->GetGameObjectByName("Soldier");
	cMesh* soldierMesh = playerAnimations.playerGameObject->GetComponent<cMesh>(); 
	TransformComponent* soldierTransform = playerAnimations.playerGameObject->GetComponent<TransformComponent>();
	CharacterController* controller = playerAnimations.playerGameObject->GetComponent<CharacterController>();
	//add animation component
	playerAnimations.animInit(soldierMesh);


	float speed = 35.0f;
	cPlayerAnimations::PlayerState currentState = cPlayerAnimations::PlayerState::IDLE;
	cPlayerAnimations::PlayerState changeToState = cPlayerAnimations::PlayerState::IDLE;

	glm::vec3 cameraOffset = glm::vec3(0, 300, -500);
	glm::vec3 cameraTarget = glm::vec3(0, 150, 0);
	camera->camControl = false;
	while (!glfwWindowShouldClose(engine.window))
	{
		engine.BeginRender();

		engine.Update();

		playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", gunTransformComponent);

		glm::vec3 cameraRot = glm::vec3(camera->pitch / 100.0f, -camera->yaw / 100.0f, 0);


		//follow the tie fighter
		camera->Follow(soldierTransform->drawPosition,cameraOffset,
			soldierTransform->drawPosition + cameraTarget, cameraRot);


		ImGui::Begin("Debug");
		ImGui::DragFloat3("Camera Position", &cameraOffset[0], 0.1f);
		ImGui::DragFloat3("Look Ahead", &cameraTarget[0], 0.1f);
		ImGui::End();
		//rotate the tie fighter
		soldierTransform->setRotationFromEuler(glm::vec3(0, -camera->yaw / 100.0f, 0));

		//get the forward and right vectors of the tie fighter
		glm::vec3 forward = soldierTransform->GetForwardVector();
		forward = glm::vec3(forward.x, 0, forward.z); // Flatten the forward vector
		glm::vec3 right = soldierTransform->GetRightVector();

		//playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", sphereTransformComponent);
		//vampireAnimationSystem->AttachObjectToBone("LeftHand", sphereTransformComponent);

		if (IsKeyPressed(GLFW_KEY_W))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKFRONT);
			controller->Move(-forward * speed, engine.deltaTime);
		}
		
		if (IsKeyPressed(GLFW_KEY_S))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKBACK);
			controller->Move(forward * speed, engine.deltaTime);
		}
		
		if (IsKeyPressed(GLFW_KEY_A))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKRIGHT);
			controller->Move(right * speed, engine.deltaTime);
		
		}
		if (IsKeyPressed(GLFW_KEY_D))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKLEFT);
			controller->Move(-right * speed, engine.deltaTime);
		}

		playerAnimations.Update(engine.deltaTime);

		//playerAnimations.BlendTo(currentState, engine.deltaTime);

		engine.EndRender();
	}

	engine.ShutDown();
}
