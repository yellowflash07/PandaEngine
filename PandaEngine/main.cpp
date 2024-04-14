
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyHit = key;
    }
    if (action == GLFW_RELEASE)
    {
		keyHit = 0;
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
    const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
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

    camera->SetPosition(glm::vec3(11.0f,37.0f, 401.0f));

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

    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update();   

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", 1/engine.deltaTime);
        ImGui::End();

        if(keyHit == GLFW_KEY_SPACE)
        {
            gVehicleInputData->setDigitalAccel(false);
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(false);
            gVehicleInputData->setDigitalBrake(false);
            gVehicleInputData->setDigitalHandbrake(false);
        }

        if (keyHit == GLFW_KEY_UP)
        {
            //accelerate

            gVehicleInputData->setDigitalAccel(true);
        }
        else if (keyHit == GLFW_KEY_DOWN)
		{
			//reverse
            gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
			gVehicleInputData->setDigitalAccel(true);
		}
		else
		{
			gVehicleInputData->setDigitalAccel(false);
            gVehicleInputData->setDigitalBrake(false);
		}

        if (keyHit == GLFW_KEY_LEFT)
        {
            gVehicleInputData->setDigitalAccel(true);
            gVehicleInputData->setDigitalSteerLeft(true);
        }
        else if (keyHit == GLFW_KEY_RIGHT)
		{
            gVehicleInputData->setDigitalAccel(true);
			gVehicleInputData->setDigitalSteerRight(true);
		}
		else
		{
            gVehicleInputData->setDigitalAccel(false);
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(false);
		}

        vehicleCreator.UpdateVehicle4W(engine.deltaTime, gravity, gVehicle4W, NULL, gVehicleInputData);

        engine.EndRender();   
    }

    engine.ShutDown();
}
