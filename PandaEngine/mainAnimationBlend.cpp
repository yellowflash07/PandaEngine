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


	cPlayerAnimations playerAnimations;
	playerAnimations.playerGameObject = scene->GetGameObjectByName("Soldier");
	cMesh* soldierMesh = playerAnimations.playerGameObject->GetComponent<cMesh>(); 
	//add animation component
	playerAnimations.animInit(soldierMesh);



 
 
	float blendWeight = 0.0f;
	float blendDuration = 0.5f; // duration in seconds
	float blendIncrement = 1.0f / blendDuration; // increment per second
	int action;
	while (!glfwWindowShouldClose(engine.window))
	{
		engine.BeginRender();

		engine.Update();

		//playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", gunTransformComponent);


		if (IsKeyPressed(GLFW_KEY_W))
		{
			playerAnimations.BlendTo(cPlayerAnimations::PlayerState::WALKFRONT, engine.deltaTime);
		}

		if (IsKeyPressed(GLFW_KEY_S))
		{
			playerAnimations.BlendTo(cPlayerAnimations::PlayerState::WALKBACK, engine.deltaTime);

		}

		if (IsKeyPressed(GLFW_KEY_A))
		{
			playerAnimations.BlendTo(cPlayerAnimations::PlayerState::WALKLEFT, engine.deltaTime);

		}
		if (IsKeyPressed(GLFW_KEY_D))
		{
			playerAnimations.BlendTo(cPlayerAnimations::PlayerState::WALKRIGHT, engine.deltaTime);

		}

 

		engine.EndRender();
	}

	engine.ShutDown();
}
