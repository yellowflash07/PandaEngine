#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include "cPlayerAnimations.h"
#include "cVehicle.h"
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
	if (action == GLFW_PRESS && keyMap[key] == false)
	{
		keyHit = key;
		keyMap[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
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

	GameObject* skyBox = scene->GetGameObjectByName("SkyBox");
	TransformComponent* skyBoxTransform = nullptr;

	cPlayerAnimations playerAnimations;
	playerAnimations.playerGameObject = scene->GetGameObjectByName("Soldier");
	cMesh* soldierMesh = playerAnimations.playerGameObject->GetComponent<cMesh>(); 
	TransformComponent* soldierTransform = playerAnimations.playerGameObject->GetComponent<TransformComponent>();
	CharacterController* controller = playerAnimations.playerGameObject->GetComponent<CharacterController>();
	//add animation component
	playerAnimations.animInit(soldierMesh);


	float speed = 35.0f;

	glm::vec3 cameraOffset = glm::vec3(0, 300, -500);
	glm::vec3 cameraTarget = glm::vec3(0, 150, 0);
	glm::vec3 vehicleTarget = glm::vec3(0, 100, 0);
	//camera->camControl = false;

	bool isInsideVehicle = false;
	cVehicle vehicleClass;
	GameObject* vehicle = scene->GetGameObjectByName("Vehicle");
	TransformComponent* vehicleTransform = vehicle->GetComponent<TransformComponent>();
	vehicleClass.SetChassis(vehicleTransform);
	
	cMesh* vehicleMesh = vehicle->GetComponent<cMesh>();
	VehicleDesc vehicleDesc = vehicleClass.initVehicleDesc(vehicleMesh);

	std::vector<TransformComponent*> wheels;
	for (int i = 0; i < 4; i++)
	{
		GameObject* wheel = scene->CreateGameObject(std::to_string(i));
		wheel->AddComponent<cMesh>("carWheel.ply", "carWheel");
		TransformComponent* wheelTransform = wheel->GetComponent<TransformComponent>();
		wheelTransform->drawScale = glm::vec3(1.23, 0.75, 0.75);
		wheels.push_back(wheelTransform);

	}
	vehicleClass.SetWheel(wheels);

	vehicleClass.startPos = glm::vec3(100,100, 100);
	vehicleClass.vehicleInit();

	while (!glfwWindowShouldClose(engine.window))
	{
		engine.BeginRender();

		engine.Update();

		if (IsKeyPressed(GLFW_KEY_G))
		{
			scene->Play();
		}


		ImGui::Begin("Debug");
		ImGui::DragFloat3("Camera Position", &cameraOffset[0], 0.1f);
		ImGui::DragFloat3("Look Ahead", &cameraTarget[0], 0.1f);
		ImGui::Checkbox("Is Inside Vehicle", &isInsideVehicle);
		//ImGui::Text("Distance from vehicle %f", glm::distance(soldierTransform->drawPosition, vehicleTransform->drawPosition));
		ImGui::End();


		if (skyBox == nullptr)
		{
			skyBox = scene->GetGameObjectByName("SkyBox");
		}
		else
		{
			skyBoxTransform = skyBox->GetComponent<TransformComponent>();
			skyBoxTransform->drawPosition = camera->cameraEye;
		}

		if (glm::distance(soldierTransform->drawPosition, vehicleTransform->drawPosition) < 150.0f)
		{
			if (keyHit == GLFW_KEY_E)
			{
				isInsideVehicle = true;
				soldierMesh->bIsVisible = false;
				keyHit = 0;
			}
		}

		if (isInsideVehicle)
		{
			if (keyHit == GLFW_KEY_E)
			{
				isInsideVehicle = false;
				soldierMesh->bIsVisible = true;
				vehicleClass.brake();
				keyHit = 0;
			}
		}
		if (!isInsideVehicle)
		{
			soldierTransform->setRotationFromEuler(glm::vec3(0, -camera->yaw / 100.0f, 0));

		/*	glm::vec3 cameraRot = glm::vec3(camera->pitch / 100.0f, -camera->yaw / 100.0f, 0);
			camera->Follow(soldierTransform->drawPosition, cameraOffset,
				soldierTransform->drawPosition + cameraTarget, cameraRot);*/
			if (IsKeyPressed(GLFW_KEY_W))
			{
				playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKFRONT);
				controller->Move(soldierTransform->GetForwardVector() * speed, engine.deltaTime);
			}
			if (IsKeyPressed(GLFW_KEY_S))
			{
				playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKBACK);
				controller->Move(-soldierTransform->GetForwardVector() * speed, engine.deltaTime);
			}
			if (IsKeyPressed(GLFW_KEY_A))
			{
				playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKLEFT);
				controller->Move(soldierTransform->GetRightVector() * speed, engine.deltaTime);
			}
			if (IsKeyPressed(GLFW_KEY_D))
			{
				playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKRIGHT);
				controller->Move(-soldierTransform->GetRightVector() * speed, engine.deltaTime);
			}
			if (!IsKeyPressed(GLFW_KEY_W) && !IsKeyPressed(GLFW_KEY_S) && !IsKeyPressed(GLFW_KEY_A) && !IsKeyPressed(GLFW_KEY_D))
			{
				playerAnimations.SetState(cPlayerAnimations::PlayerState::IDLE);
				controller->Move(glm::vec3(0, 0, 0), engine.deltaTime);
			}
		}
		else
		{
		/*	glm::vec3 cameraRot = glm::vec3(camera->pitch / 50.0f, -camera->yaw / 50.0f, 0);
			camera->Follow(vehicleTransform->drawPosition, cameraOffset,
				vehicleTransform->drawPosition + vehicleTarget, cameraRot);*/

			glm::vec3 soldierPos = vehicleTransform->drawPosition + glm::vec3(300,0, 0);
			controller->controller->setPosition(PxExtendedVec3(soldierPos.x, soldierPos.y, soldierPos.z));

			//player is in vehicle
			if (IsKeyPressed(GLFW_KEY_W))
			{
				vehicleClass.moveForward();
			}
			if (IsKeyPressed(GLFW_KEY_S))
			{
				vehicleClass.moveBackward();
			}

			if (IsKeyPressed(GLFW_KEY_A))
			{
				vehicleClass.turnLeft();
			}
			else if (IsKeyPressed(GLFW_KEY_D))
			{
				vehicleClass.turnRight();
			}
			else
			{
				vehicleClass.stopTurning();
			}

			if (IsKeyPressed(GLFW_KEY_SPACE))
			{
				vehicleClass.brake();
			}

			if (IsKeyPressed(GLFW_KEY_R))
			{
				vehicleClass.reset();
			}
			
		}

		vehicleClass.Update(engine.deltaTime);

		playerAnimations.Update(engine.deltaTime);

		engine.EndRender();
	}

	engine.ShutDown();
}


