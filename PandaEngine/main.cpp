
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
extern Camera* camera;
int keyHit = 0;

#include "Scene.h"
std::map<int,bool> keys;

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

VehicleDesc initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const PxF32 chassisMass = 1500.0f;
    const PxVec3 chassisDims(3.5f, 1.0f, 9.0f);
    const PxVec3 chassisMOI
    ((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
        (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
        (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = 20.0f;
    const PxF32 wheelRadius = 0.5f;
    const PxF32 wheelWidth = 0.4f;
    const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
    const PxU32 nbWheels = 4;

    VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = PhysXManager::getInstance()->gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = PhysXManager::getInstance()->gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
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

    camera->SetPosition(glm::vec3(11.0f,37.0f,19.0f));

    engine.LoadSave(); 
    printf("DONE\n");
    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();


    VehicleCreator vehicleCreator;
    VehicleDesc vehicleDesc = initVehicleDesc();
    PxVehicleDrive4W* gVehicle4W = vehicleCreator.CreateVehicle4W(vehicleDesc, PhysXManager::getInstance()->gPhysics, PhysXManager::getInstance()->gCooking);
    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
    gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
    PhysXManager::getInstance()->gScene->addActor(*gVehicle4W->getRigidDynamicActor());

    //Set the vehicle to rest in first gear.
    //Set the vehicle to use auto-gears.
    gVehicle4W->setToRestState();
    gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    gVehicle4W->mDriveDynData.setUseAutoGears(true);

    PxVec3 gravity = PhysXManager::getInstance()->gScene->getGravity();
    PxVehicleDrive4WRawInputData* gVehicleInputData = new PxVehicleDrive4WRawInputData();

    gVehicleInputData->setDigitalAccel(false);
    gVehicleInputData->setDigitalSteerLeft(false);
    gVehicleInputData->setDigitalSteerRight(false);
    gVehicleInputData->setDigitalBrake(false);
    gVehicleInputData->setDigitalHandbrake(false);

    GameObject* vehicle = scene->GetGameObjectByName("Vehicle");
    TransformComponent* vehicleTransform = vehicle->GetComponent<TransformComponent>();
    std::vector<TransformComponent*> wheels;
    for (int i = 0; i < 4; i++)
    {
        GameObject* wheel = scene->CreateGameObject(std::to_string(i));
        wheel->AddComponent<cMesh>("carWheel.ply", "carWheel");
        TransformComponent* wheelTransform = wheel->GetComponent<TransformComponent>();
        wheelTransform->drawScale = glm::vec3(0.02);
        wheels.push_back(wheelTransform);

    }

    //glm::vec3 position = glm::vec3(gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.z);
    //glm::quat rotation = glm::quat(gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.w, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.z);
    //TransformComponent* transform = vehicle->GetComponent<TransformComponent>();
    //transform->drawPosition = position;
    //cMesh* vehicleMesh = &vehicle->AddComponent<cMesh>("carBody.fbx", "carBody");
    bool hasReversed = false;
    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update();   

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", 1/engine.deltaTime);
        ImGui::End();
        //position = glm::vec3(gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.z);
        //rotation = glm::quat(gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.w, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.z);
        //transform->drawPosition = position;
        //transform->eulerRotation = glm::eulerAngles(rotation);
        if(IsKeyPressed(GLFW_KEY_SPACE))
        {
            gVehicleInputData->setDigitalAccel(false);
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(false);
            gVehicleInputData->setDigitalBrake(false);
            gVehicleInputData->setDigitalHandbrake(false);
        }

        if (IsKeyPressed(GLFW_KEY_UP))
        {
            //accelerate
            if (hasReversed)
            {
				gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				hasReversed = false;
            }
            gVehicleInputData->setDigitalAccel(true);
        }
        if (IsKeyPressed(GLFW_KEY_DOWN))
		{
			//reverse
            gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
			gVehicleInputData->setDigitalAccel(true);
            hasReversed = true;
		}

        if (IsKeyPressed(GLFW_KEY_LEFT))
        {
            gVehicleInputData->setDigitalSteerLeft(true);
            gVehicleInputData->setDigitalSteerRight(false);
        }
        if (IsKeyPressed(GLFW_KEY_RIGHT))
		{
            gVehicleInputData->setDigitalSteerLeft(false);
			gVehicleInputData->setDigitalSteerRight(true);
		}
		

        vehicleCreator.UpdateVehicle4W(engine.deltaTime, gravity, gVehicle4W, NULL, gVehicleInputData);

        int numShapes = gVehicle4W->getRigidDynamicActor()->getNbShapes();
        for (PxU32 i = 0; i < numShapes; i++)
        {
            PxShape* shape;
            gVehicle4W->getRigidDynamicActor()->getShapes(&shape, 1, i);

            CarData* carData = (CarData*)shape->userData;

            //wheelShape->userData = wheelData;
            if (carData != nullptr)
            {
                PxTransform shapePose = PxShapeExt::getGlobalPose(*shape, *gVehicle4W->getRigidDynamicActor());
                glm::vec3 pos = glm::vec3(shapePose.p.x, shapePose.p.y, shapePose.p.z);

                if (carData->carPart == WHEEL)
                {
					wheels[carData->index]->drawPosition = pos;
					wheels[carData->index]->eulerRotation = glm::eulerAngles(glm::quat(shapePose.q.w, shapePose.q.x, shapePose.q.y, shapePose.q.z));
                }
                if(carData->carPart == CHASSIS)
				{
					vehicleTransform->drawPosition = pos;
					vehicleTransform->eulerRotation = glm::eulerAngles(glm::quat(shapePose.q.w, shapePose.q.x, shapePose.q.y, shapePose.q.z));
				}
            }
        }

        engine.EndRender();   
    }

    engine.ShutDown();
}
