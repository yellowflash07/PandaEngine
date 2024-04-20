#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include "cPlayerAnimations.h"
#include "EnemyManager.h"
#include "cVehicle.h"
#include "Raycast.h"
extern Camera* camera;
extern bool IMGUI_ENABLE;
int keyHit = 0;
int mouseKeyHit = 0;
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
		keyMap[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera->ProcessMouseMovement(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		mouseKeyHit = button;
	}
	if (action == GLFW_RELEASE)
	{
		mouseKeyHit = 0;
	}
}


int main(void)
{
	Engine engine;
	if (!engine.Initialize())
	{
		return 1;
	}

	bool loaded = engine.meshManager->LoadCubeMap("space",
		"CubeMaps/px.png", 
		"CubeMaps/nx.png", 
		"CubeMaps/py.png", 
		"CubeMaps/ny.png", 
		"CubeMaps/pz.png", 
		"CubeMaps/nz.png", true);

	camera->SetPosition(glm::vec3(11.0f, 158.0f, 401.0f));

	engine.LoadSave("AnimationScene.json");

	float currTime = 0;
	float myTime = 0;

	bool playMode = false;

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

	GameObject* gun = scene->GetGameObjectByName("Gun");

	float speed = 100.0f;

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
	EnemyManager enemyManager(scene, scene->GetGameObjectByName("Soldier"));
	//enemyManager.m_Scene = scene;
	//enemyManager.CreateEnemy();

	std::vector<TransformComponent*> wheels;
	for (int i = 0; i < 4; i++)
	{
		GameObject* wheel = scene->GetGameObjectByName(std::to_string(i));
		TransformComponent* wheelTransform = wheel->GetComponent<TransformComponent>();
		wheels.push_back(wheelTransform);

	}
	vehicleClass.SetWheel(wheels);

	vehicleClass.startPos = glm::vec3(-1700,50, 76);
	vehicleClass.vehicleInit();

	while (!glfwWindowShouldClose(engine.window))
	{
		engine.BeginRender();

		engine.Update();

		if (IMGUI_ENABLE)
		{
			if (IsKeyPressed(GLFW_KEY_G))
			{
				scene->Play();
			}
			if (IsKeyPressed(GLFW_KEY_H))
			{
				scene->Stop();
			}
		}
		else
		{
			scene->Play();
		}
		

		if (skyBox == nullptr)
		{
			skyBox = scene->GetGameObjectByName("SkyBox");
		}
		else
		{
			skyBoxTransform = skyBox->GetComponent<TransformComponent>();
			skyBoxTransform->drawPosition = camera->cameraEye;
		}

		if (scene->play)
		{
			if (gun == nullptr)
			{
				gun = scene->GetGameObjectByName("Gun");
			}
			else
			{
				TransformComponent* gunTransform = gun->GetComponent<TransformComponent>();
				playerAnimations.animationSystem->AttachObjectToBone("mixamorig_LeftHand", gunTransform);

				glm::vec3 gunPos = glm::vec3(gunTransform->parentTransform[3]) + gunTransform->drawPosition;
				glm::vec3 rayDir = soldierTransform->GetForwardVector();
				Raycast raycast(gunPos, rayDir, 10000.0f);
				HitResult hitResult;
				if (raycast.RaycastHit(hitResult))
				{
					if (hitResult.hitActor != nullptr)
					{
						GameObject* hitObject = (GameObject*)hitResult.hitActor->GameObject;
						if (hitObject != nullptr)
						{
							if (hitObject->tag == "Zombie")
							{
								if (glfwGetMouseButton(engine.window, GLFW_MOUSE_BUTTON_LEFT))
								{
									enemyManager.RemoveEnemy(hitObject);
									mouseKeyHit = 0;
								}
							}
						}
					}
				}
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
					controller->StopHold();
					keyHit = 0;
				}
			}
			if (!isInsideVehicle)
			{
				enemyManager.Update(engine.deltaTime);

				soldierTransform->setRotationFromEuler(glm::vec3(0, -camera->yaw / 100.0f, 0));

				glm::vec3 cameraRot = glm::vec3(camera->pitch / 100.0f, -camera->yaw / 100.0f, 0);
				camera->Follow(soldierTransform->drawPosition, cameraOffset,
					soldierTransform->drawPosition + cameraTarget, cameraRot);
				if (!IsKeyPressed(GLFW_KEY_W) && !IsKeyPressed(GLFW_KEY_S) && !IsKeyPressed(GLFW_KEY_A) && !IsKeyPressed(GLFW_KEY_D) && !glfwGetMouseButton(engine.window, GLFW_MOUSE_BUTTON_LEFT))
				{
					playerAnimations.SetState(cPlayerAnimations::PlayerState::IDLE);
					controller->Move(glm::vec3(0, 0, 0), engine.deltaTime);
				}
				else
				{
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
						controller->Move(-soldierTransform->GetRightVector() * speed, engine.deltaTime);
					}
					if (IsKeyPressed(GLFW_KEY_D))
					{
						playerAnimations.SetState(cPlayerAnimations::PlayerState::WALKRIGHT);
						controller->Move(soldierTransform->GetRightVector() * speed, engine.deltaTime);
					}
					if (glfwGetMouseButton(engine.window, GLFW_MOUSE_BUTTON_LEFT))
					{
						playerAnimations.SetState(cPlayerAnimations::PlayerState::SHOOT);
						controller->Move(glm::vec3(0, 0, 0), engine.deltaTime);
					}
				}

			}
			else
			{
				glm::vec3 cameraRot = glm::vec3(camera->pitch / 50.0f, -camera->yaw / 50.0f, 0);
				camera->Follow(vehicleTransform->drawPosition, cameraOffset,
					vehicleTransform->drawPosition + vehicleTarget, cameraRot);
				glm::vec3 vehicleForward = vehicleTransform->GetForwardVector();
				glm::vec3 soldierForwardPos = vehicleTransform->drawPosition + vehicleForward * 200.0f;
				glm::vec3 soldierPos = glm::vec3(soldierForwardPos.x, 100.0f, soldierForwardPos.z);
				controller->Hold(soldierPos);

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

			
		}
		else
		{
			camera->isFollowing = false;
		}
		

		engine.EndRender();
	}

	engine.ShutDown();
}


