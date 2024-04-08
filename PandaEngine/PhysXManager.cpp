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

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return PxFilterFlag::eDEFAULT;
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
					std::map< PxActor*, PhysXBody*>::iterator
						otherActor = ::gActorMap.find(current.otherActor);
					PhysXBody* other = nullptr;
					if (itActor == ::gActorMap.end())
					{
						other = nullptr;
					}
					else
					{
						PhysXBody* otherBody = &(*otherActor->second);
						other = otherBody;
					}
					itActor->second->onTriggerEnter(other);
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
					std::map< PxActor*, PhysXBody*>::iterator
						otherActor = ::gActorMap.find(current.otherActor);
					PhysXBody* other = nullptr;
					if (itActor == ::gActorMap.end())
					{
						other = nullptr;
					}
					else
					{
						PhysXBody* otherBody = &(*otherActor->second);
						other = otherBody;
					}
					itActor->second->onTriggerExit(other);
				}
			}
		
		}
	}

	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
	{
		printf("onAdvance\n");
	}

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count)
	{
		
		//		printf("onContact: %d pairs\n", count);
		std::vector<PxContactPairPoint> contactPoints;
		for (PxU32 i = 0; i < count; i++)
		{
			PxU32 contactCount = pairs[i].contactCount;
			if (contactCount)
			{
				contactPoints.resize(contactCount);
				pairs[i].extractContacts(&contactPoints[0], contactCount);

				for (PxU32 j = 0; j < contactCount; j++)
				{
					std::map< PxActor*, PhysXBody*>::iterator
						itActor = ::gActorMap.find(pairHeader.actors[0]);


					if (itActor == ::gActorMap.end())
					{
						// Didn't find it
						return;		// or 0 or nullptr
					}

					if (itActor->second->onContactStart)
					{
						itActor->second->onContactStart(glm::vec3(contactPoints[j].position.x, contactPoints[j].position.y, contactPoints[j].position.z));
					}

				}
			}
		}

		//while (count--)
		//{
		//	const PxContactPair& current = *pairs++;

		//	// The reported pairs can be trigger pairs or not. We only enabled contact reports for
		//	// trigger pairs in the filter shader, so we don't need to do further checks here. In a
		//	// real-world scenario you would probably need a way to tell whether one of the shapes
		//	// is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
		//	// shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
		//	// in a hash-set and test the reported shape pointers against it. Many options here.

		//	if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
		//		printf("Shape is entering trigger volume\n");
		//	if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		//		printf("Shape is leaving trigger volume\n");

		///*	if (isTriggerShape(current.shapes[0]) && isTriggerShape(current.shapes[1]))
		//		printf("Trigger-trigger overlap detected\n");*/
		//}
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
   // sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.filterShader = contactReportFilterShader;
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

