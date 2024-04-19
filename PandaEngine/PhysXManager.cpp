#include "PhysXManager.h"
#include "PhysXBody.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "PhysXVehicleEnums.h"

static PhysXManager* gPhysXManager = NULL;
extern std::map<PxActor*, PhysXBody*> gActorMap;


//Contact modification values.
#define WHEEL_TANGENT_VELOCITY_MULTIPLIER 0.1f
#define MAX_IMPULSE PX_MAX_F32

//PhysX Vehicles support blocking and non-blocking sweeps.
//Experiment with this define to switch between the two regimes.
#define BLOCKING_SWEEPS 0


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

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

	return PxFilterFlags();
}


PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
//	PX_UNUSED(filterData0);
//	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;

	/*if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
		return PxFilterFlag::eSUPPRESS;*/

	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));

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


//The class WheelContactModifyCallback identifies and modifies rigid body contacts
//that involve a wheel.  Contacts that can be identified and managed by the suspension
//system are ignored.  Any contacts that remain are modified to account for the rotation
//speed of the wheel around the rolling axis.
class WheelContactModifyCallback : public PxContactModifyCallback
{
public:

	WheelContactModifyCallback()
		: PxContactModifyCallback()
	{
	}

	~WheelContactModifyCallback() {}

	void onContactModify(PxContactModifyPair* const pairs, PxU32 count)
	{
		int breakHere = 0;
		for (PxU32 i = 0; i < count; i++)
		{
			const PxRigidActor** actors = pairs[i].actor;
			const PxShape** shapes = pairs[i].shape;

			//Search for actors that represent vehicles and shapes that represent wheels.
			for (PxU32 j = 0; j < 2; j++)
			{
				const PxActor* actor = actors[j];
				if (actor->userData && (static_cast<ActorUserData*>(actor->userData))->vehicle)
				{
					const PxVehicleWheels* vehicle = (static_cast<ActorUserData*>(actor->userData))->vehicle;
					PX_ASSERT(vehicle->getRigidDynamicActor() == actors[j]);

					const PxShape* shape = shapes[j];
					if (shape->userData && (static_cast<ShapeUserData*>(shape->userData))->isWheel)
					{
						const PxU32 wheelId = (static_cast<ShapeUserData*>(shape->userData))->wheelId;
						PX_ASSERT(wheelId < vehicle->mWheelsSimData.getNbWheels());

						//Modify wheel contacts.
						PxVehicleModifyWheelContacts(*vehicle, wheelId, WHEEL_TANGENT_VELOCITY_MULTIPLIER, MAX_IMPULSE, pairs[i]);
					}
				}
			}
		}
	}

private:

};
WheelContactModifyCallback gWheelContactModifyCallback;

class WheelCCDContactModifyCallback : public PxCCDContactModifyCallback
{
public:

	WheelCCDContactModifyCallback()
		: PxCCDContactModifyCallback()
	{
	}

	~WheelCCDContactModifyCallback() {}

	void onCCDContactModify(PxContactModifyPair* const pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			const PxRigidActor** actors = pairs[i].actor;
			const PxShape** shapes = pairs[i].shape;

			//Search for actors that represent vehicles and shapes that represent wheels.
			for (PxU32 j = 0; j < 2; j++)
			{
				const PxActor* actor = actors[j];
				if (actor->userData && (static_cast<ActorUserData*>(actor->userData))->vehicle)
				{
					const PxVehicleWheels* vehicle = (static_cast<ActorUserData*>(actor->userData))->vehicle;
					PX_ASSERT(vehicle->getRigidDynamicActor() == actors[j]);

					const PxShape* shape = shapes[j];
					if (shape->userData && (static_cast<ShapeUserData*>(shape->userData))->isWheel)
					{
						const PxU32 wheelId = (static_cast<ShapeUserData*>(shape->userData))->wheelId;
						PX_ASSERT(wheelId < vehicle->mWheelsSimData.getNbWheels());

						//Modify wheel contacts.
						PxVehicleModifyWheelContacts(*vehicle, wheelId, WHEEL_TANGENT_VELOCITY_MULTIPLIER, MAX_IMPULSE, pairs[i]);
					}
				}
			}
		}
	}
};
WheelCCDContactModifyCallback gWheelCCDContactModifyCallback;
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
	PxTolerancesScale scale;
	scale.length = 18;
	scale.speed = 10.0f * 18;
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale, true, gPvd);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f * 18, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
   // sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.contactModifyCallback = &gWheelContactModifyCallback;
	sceneDesc.ccdContactModifyCallback = &gWheelCCDContactModifyCallback;	//Enable ccd contact modification

	sceneDesc.simulationEventCallback = &gContactReportCallback;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    gScene = gPhysics->createScene(sceneDesc);


	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_POINT, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_NORMAL, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_ERROR, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_FORCE, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

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
    gScene->simulate(deltaTime);
    gScene->fetchResults(true);
}

void PhysXManager::DebugUpdate(float deltaTime)
{
	if (updateDebug)
	{
		gScene->simulate(1.0f / 3000.0f);
		gScene->fetchResults(true);
		updateDebug = false;
	}
	
}

void PhysXManager::DrawDebug()
{
	const PxRenderBuffer& rb = gScene->getRenderBuffer();

	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];

		glm::vec3 pos0 = glm::vec3(line.pos0.x, line.pos0.y, line.pos0.z);
		glm::vec3 pos1 = glm::vec3(line.pos1.x, line.pos1.y, line.pos1.z);
		glm::vec3 color0 = glm::vec3(line.color0);
		glm::vec3 color1 = glm::vec3(line.color1);

		DebugRenderer::getInstance()->DrawLine(pos0, pos1, glm::vec3(1,0,0), glm::vec3(1, 0, 0));
	}

}

void PhysXManager::Shutdown()
{
}
