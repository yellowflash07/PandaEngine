#include "PhysXManager.h"
#include "PhysXBody.h"

static PhysXManager* gPhysXManager = NULL;
extern std::map<PxActor*, PhysXBody*> gActorMap;

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
class ContactReportCallback : public PxSimulationEventCallback
{
	void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/)
	{
		printf("onConstraintBreak\n");
	}

	void onWake(PxActor** /*actors*/, PxU32 /*count*/)
	{
		printf("onWake\n");
	}

	void onSleep(PxActor** /*actors*/, PxU32 /*count*/)
	{
		printf("onSleep\n");
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		//		printf("onTrigger: %d trigger pairs\n", count);
		while (count--)
		{
			const PxTriggerPair& current = *pairs++;
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				std::map< PxActor*, PhysXBody*>::iterator
					itActor = ::gActorMap.find(current.triggerActor);
				if ( itActor == ::gActorMap.end() )
				{
					// Didn't find it
					return;		// or 0 or nullptr
				}

				if (itActor->second->onTriggerEnter)
				{
					itActor->second->onTriggerEnter();
				}

				//printf("Shape is entering trigger volume\n");
			}
				
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				std::map< PxActor*, PhysXBody*>::iterator
					itActor = ::gActorMap.find(current.triggerActor);
				if (itActor == ::gActorMap.end())
				{
					// Didn't find it
					return;		// or 0 or nullptr
				}

				if (itActor->second->onTriggerExit)
				{
					itActor->second->onTriggerExit();
				}
			}
		
		}
	}

	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
	{
		printf("onAdvance\n");
	}

	void onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count)
	{
		//		printf("onContact: %d pairs\n", count);

		while (count--)
		{
			const PxContactPair& current = *pairs++;

			// The reported pairs can be trigger pairs or not. We only enabled contact reports for
			// trigger pairs in the filter shader, so we don't need to do further checks here. In a
			// real-world scenario you would probably need a way to tell whether one of the shapes
			// is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
			// shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
			// in a hash-set and test the reported shape pointers against it. Many options here.

			if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
				printf("Shape is entering trigger volume\n");
			if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				printf("Shape is leaving trigger volume\n");

		/*	if (isTriggerShape(current.shapes[0]) && isTriggerShape(current.shapes[1]))
				printf("Trigger-trigger overlap detected\n");*/
		}
	}
};

static ContactReportCallback gContactReportCallback;

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
	sceneDesc.simulationEventCallback = &gContactReportCallback;
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

