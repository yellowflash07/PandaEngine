
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>
#include "PhysXManager.h"
#include "PhysXBody.h"
#include "Raycast.h"
#include <glm/gtx/string_cast.hpp>
#include "Debug.h"
#include "VehicleCreator.h"
#include "cVehicle.h"

extern Camera* camera;
int keyHit = 0;

#include "Scene.h"
#include <PhysX/vehicle/PxVehicleUtil.h>
std::map<int, bool> keys;

bool IsKeyPressed(int key)
{
    return keys[key];
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyHit = key;
        keys[key] = true;
    }
    if (action == GLFW_RELEASE)
    {
        keyHit = 0;
        keys[key] = false;
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

    camera->SetPosition(glm::vec3(11.0f, 50.0f, 444.0f));

    engine.LoadSave("Scene_Vehicle.json");
    printf("DONE\n");
    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();

    GameObject* ground = scene->GetGameObjectByName("Ground");
    PhysXBody* groundBody = ground->GetComponent<PhysXBody>();

    GameObject* Terrain = scene->GetGameObjectByName("Terrain");
    PhysXBody* TerrainBody = ground->GetComponent<PhysXBody>();

    GameObject* Obstacle = scene->GetGameObjectByName("Obstacle");
    PhysXBody* ObstacleBody = ground->GetComponent<PhysXBody>();

    //vehicle.chassisTransformComponent
    cVehicle vehicleClass;


    GameObject* vehicle = scene->GetGameObjectByName("Vehicle");
    vehicleClass.SetChassis(vehicle->GetComponent<TransformComponent>());
    //TransformComponent* vehicleTransform = vehicle->GetComponent<TransformComponent>();

    // Assuming vehicle is an instance of cVehicle
    TransformComponent* vehicleTransform = vehicleClass.GetChassis();
    cMesh* vehicleMesh = vehicle->GetComponent<cMesh>();
    VehicleDesc vehicleDesc = vehicleClass.initVehicleDesc(vehicleMesh);


    //vehicle.chassisTransformComponent = 4
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


    vehicleClass.vehicleInit();
 
    glm::vec3 positionDifference (150.0f,50.0f,150.0f);
    while (!glfwWindowShouldClose(engine.window))
    {
        //  PxVehicleWheelsDynData::getTireLongSlip

        engine.BeginRender();

        engine.Update();

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", 1 / engine.deltaTime);
        //ImGui::Text("Speed: %f\n", gVehicle4W->computeForwardSpeed());

        //ImGui::DragFloat("X Offset", &vehicleXoffset, 0.1f);
        //ImGui::DragFloat("Y Offset", &vehicleYoffset, 0.1f);
        //ImGui::DragFloat("Z Offset", &vehicleZoffset, 0.1f);
        ImGui::End();
        //glm ::vec3 vehicleRotation = vehicleTransform->eulerRotation;
        //glm::vec3 cameraRot = glm::vec3(-camera->pitch, camera->yaw, 0.0f); // Adjust according to your needs
        //camera->Follow(vehicleTransform->drawPosition, glm::vec3(1, 2, -5), vehicleTransform->drawPosition, vehicleRotation + cameraRot);
 

        glm::vec3 cameraRot = glm::vec3(-camera->pitch / 10.0f, camera->yaw / 10.0f, 0);
        camera->Follow(vehicleTransform->drawPosition , glm::vec3(200, 55, -400), vehicleTransform->drawPosition, cameraRot);

        /*     position = glm::vec3(gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.z);
             rotation = glm::quat(gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.w, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.z);
             transform->drawPosition = position;
             transform->eulerRotation = glm::eulerAngles(rotation);*/

        if (IsKeyPressed(GLFW_KEY_SPACE))
        {
    
            vehicleClass.reset();
        }

        if (IsKeyPressed(GLFW_KEY_UP))
        {
          
            vehicleClass.moveForward();
        }
        if (IsKeyPressed(GLFW_KEY_DOWN))
        {      

            vehicleClass.moveBackward();
        }

        //moveleft right (bool left, bool right)
        if (IsKeyPressed(GLFW_KEY_LEFT))
        {
            vehicleClass.turnLeft();
        }
        else if (IsKeyPressed(GLFW_KEY_RIGHT))
        {

    
            vehicleClass.turnRight();
        }
        else
        {
            vehicleClass.stopTurning();
        }


        vehicleClass.Update(engine.deltaTime);
        
        //update end
        engine.EndRender();
    }

    engine.ShutDown();
}
