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
	//add animation component
	playerAnimations.animInit(soldierMesh);

	cPlayerAnimations::PlayerState currentState = cPlayerAnimations::PlayerState::IDLE;
	cPlayerAnimations::PlayerState changeToState = cPlayerAnimations::PlayerState::IDLE;
	while (!glfwWindowShouldClose(engine.window))
	{
		engine.BeginRender();

		engine.Update();

		playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", gunTransformComponent);
		//playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", sphereTransformComponent);
		//vampireAnimationSystem->AttachObjectToBone("LeftHand", sphereTransformComponent);

		if (IsKeyPressed(GLFW_KEY_W))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKFRONT);
		}
		
		if (IsKeyPressed(GLFW_KEY_S))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKBACK);
		
		}
		
		if (IsKeyPressed(GLFW_KEY_A))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKRIGHT);
		
		}
		if (IsKeyPressed(GLFW_KEY_D))
		{
			playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKLEFT);
		}

		playerAnimations.Update(engine.deltaTime);

		//playerAnimations.BlendTo(currentState, engine.deltaTime);

		engine.EndRender();
	}

	engine.ShutDown();
}
