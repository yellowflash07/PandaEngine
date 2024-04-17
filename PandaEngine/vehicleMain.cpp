
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
#include <PhysX/vehicle/PxVehicleUtil.h>
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

//vehicle 

//init vehicle
VehicleDesc initVehicleDesc(cMesh* chassisMesh)
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const PxF32 chassisMass = 1500.0f;

    chassisMesh->calcExtents();

   // glm::vec3 center = chassisMesh->maxExtents_XYZ - chassisMesh->minExtents_XYZ;
   // PxVec3 chassisDims(center.x, center.y, center.z);
    const PxVec3 chassisDims(4.5f, 1.0f , 5.0f);
   // const PxVec3 chassisDims(4.5f, 2.0f, 9.0f);
    const PxVec3 chassisMOI
    ((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12,
        (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) /** 0.8f*/ * chassisMass / 12,
        (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y)  * chassisMass / 12);
  //  const PxVec3 chassisMOI(1000, 800, 1000);
    //fo
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f - 1.65f, -0.75f);
  //  const PxVec3 chassisCMOffset(0.0f, 0.0, 0.25f);




    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = 20.0f ;
    const PxF32 wheelRadius = 1.38f;
 //   const PxF32 wheelRadius = 0.5f;
    const PxF32 wheelWidth =  0.85f;
   // const PxF32 wheelWidth =  0.4f;
    const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
    const PxU32 nbWheels = 4;

    VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;

    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = PhysXManager::getInstance()->gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
   // vehicleDesc.chassisMesh = chassisMesh;

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = PhysXManager::getInstance()->gMaterial;
    vehicleDesc.axleWidth = 0.0f;
    vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eMODIFY_CONTACTS, 0);

    vehicleDesc.actorUserData = new ActorUserData();
    vehicleDesc.shapeUserDatas = new ShapeUserData[nbWheels];

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

    camera->SetPosition(glm::vec3(11.0f,50.0f,444.0f));

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

    GameObject* vehicle = scene->GetGameObjectByName("Vehicle");
    TransformComponent* vehicleTransform = vehicle->GetComponent<TransformComponent>();
    cMesh* vehicleMesh = vehicle->GetComponent<cMesh>();

    //vehicle.chassisTransformComponent = 4
    std::vector<TransformComponent*> wheels;
    for (int i = 0; i < 4; i++)
    {
        GameObject* wheel = scene->CreateGameObject(std::to_string(i));
        wheel->AddComponent<cMesh>("carWheel.ply", "carWheel");
        TransformComponent* wheelTransform = wheel->GetComponent<TransformComponent>();
        wheelTransform->drawScale = glm::vec3(1.23, 0.75,0.75);
        wheels.push_back(wheelTransform);

    }

    //call this at init
    VehicleCreator vehicleCreator;
    VehicleDesc vehicleDesc = initVehicleDesc(vehicleMesh);
    PxVehicleDrive4W* gVehicle4W = vehicleCreator.CreateVehicle4W(vehicleDesc, PhysXManager::getInstance()->gPhysics, PhysXManager::getInstance()->gCooking);
    
    
    float gLengthScale = 18.0f;
    vehicleCreator.customizeVehicleToLengthScale(gLengthScale, gVehicle4W->getRigidDynamicActor(), &gVehicle4W->mWheelsSimData, &gVehicle4W->mDriveSimData);

  //  PxTransform startTransform(PxVec3(0,-10, 20), PxQuat(PxIdentity));
    PxTransform startTransform(PxVec3(-330, -800,1400), PxQuat(PxIdentity));
    gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
    gVehicle4W->getRigidDynamicActor()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
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

    //end init

    float vehicleYoffset = vehicleTransform->drawPosition.y;
    float vehicleXoffset = vehicleTransform->drawPosition.x;
    float vehicleZoffset = vehicleTransform->drawPosition.z;
    //member variable
    float vehicleXrot = -1.6f;

    bool hasReversed = false;
    while (!glfwWindowShouldClose(engine.window))
    {
      //  PxVehicleWheelsDynData::getTireLongSlip

        engine.BeginRender();

        engine.Update();   

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", 1/engine.deltaTime);
        ImGui::Text("Speed: %f\n", gVehicle4W->computeForwardSpeed());

        //ImGui::DragFloat("X Offset", &vehicleXoffset, 0.1f);
        //ImGui::DragFloat("Y Offset", &vehicleYoffset, 0.1f);
        //ImGui::DragFloat("Z Offset", &vehicleZoffset, 0.1f);
        ImGui::End();
        glm::vec3 cameraRot = glm::vec3(camera->pitch / 100.0f, -camera->yaw / 100.0f, 0);
        camera->Follow(vehicleTransform->drawPosition, glm::vec3(1, 2, -5), vehicleTransform->drawPosition, cameraRot);

        //position = glm::vec3(gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().p.z);
        //rotation = glm::quat(gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.w, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.x, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.y, gVehicle4W->getRigidDynamicActor()->getGlobalPose().q.z);
        //transform->drawPosition = position;
        //transform->eulerRotation = glm::eulerAngles(rotation);
        if(IsKeyPressed(GLFW_KEY_SPACE))
        {
            //reset function
            gVehicle4W->setToRestState();
            gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
            gVehicleInputData->setDigitalAccel(false);
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(false);
            gVehicleInputData->setDigitalBrake(false);
            gVehicleInputData->setDigitalHandbrake(false);
            PxTransform currentTransform(gVehicle4W->getRigidDynamicActor()->getGlobalPose().p, PxQuat(PxIdentity));
            gVehicle4W->getRigidDynamicActor()->setGlobalPose(currentTransform);
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

        //moveleft right (bool left, bool right)
        if (IsKeyPressed(GLFW_KEY_LEFT))
        {
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(true);
        }
        else if (IsKeyPressed(GLFW_KEY_RIGHT))
		{
           
            gVehicleInputData->setDigitalSteerLeft(true);
            gVehicleInputData->setDigitalSteerRight(false);
		}
        else
        {
            gVehicleInputData->setDigitalSteerLeft(false);
            gVehicleInputData->setDigitalSteerRight(false);
        }
		
        //udpate start
        vehicleCreator.UpdateVehicle4W(engine.deltaTime, gravity, gVehicle4W, NULL, gVehicleInputData);

        int numShapes = gVehicle4W->getRigidDynamicActor()->getNbShapes();
        for (PxU32 i = 0; i < numShapes; i++)
        {
            PxShape* shape;
            gVehicle4W->getRigidDynamicActor()->getShapes(&shape, 1, i);
        
            ShapeUserData* carData = (ShapeUserData*)shape->userData;
        
            //wheelShape->userData = wheelData;
            if (carData != nullptr)
            {
                PxTransform shapePose = PxShapeExt::getGlobalPose(*shape, *gVehicle4W->getRigidDynamicActor());
                glm::vec3 pos = glm::vec3(shapePose.p.x, shapePose.p.y, shapePose.p.z);
                glm::vec3 rotation = glm::eulerAngles(glm::quat(shapePose.q.w, shapePose.q.x, shapePose.q.y, shapePose.q.z));
                if (carData->isWheel)
                {
                    glm::vec3 finalPosition = glm::vec3(pos.x + vehicleXoffset, pos.y, pos.z + vehicleZoffset);
					wheels[carData->wheelId]->drawPosition = pos;
                    glm::vec3 finalRotation = glm::vec3(rotation.x , rotation.y, rotation.z );
					wheels[carData->wheelId]->eulerRotation = finalRotation;
                }
                if(!carData->isWheel && carData->wheelId == -2)
				{
                   // vehicleYoffset += pos.y;
					vehicleTransform->drawPosition = glm::vec3(pos.x, pos.y, pos.z);
                    glm::vec3 finalRotation = glm::vec3(rotation.x + vehicleXrot, rotation.y, rotation.z);
					vehicleTransform->eulerRotation = finalRotation;
				}
            }
        }
        //update end
        engine.EndRender();   
    }

    engine.ShutDown();
}
