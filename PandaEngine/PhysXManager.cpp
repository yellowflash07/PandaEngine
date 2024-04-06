#include "PhysXManager.h"

static PhysXManager* gPhysXManager = NULL;

PhysXManager::PhysXManager()
{
}

PhysXManager::~PhysXManager()
{
}

PhysXManager* PhysXManager::getInstance()
{
    if (gPhysXManager == NULL)
    {
        gPhysXManager = new PhysXManager();
    }
    return gPhysXManager;
}

void PhysXManager::Init(bool connectToPvd)
{
    if(initialized)
		return;
    initialized = true;

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    if (connectToPvd)
    {
        gPvd = PxCreatePvd(*gFoundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    }
    
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
}

void PhysXManager::Update(float deltaTime)
{
    gScene->simulate(1.0f / 60.0f);
    gScene->fetchResults(true);
}

void PhysXManager::Shutdown()
{
}

