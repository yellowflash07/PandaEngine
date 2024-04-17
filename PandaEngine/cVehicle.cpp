#include "cVehicle.h"


cVehicle::cVehicle()
{
 
}

cVehicle::~cVehicle()
{
}

void cVehicle::vehicleInit()
{


	gVehicle4W = vehicleCreator.CreateVehicle4W(vehicleDesc, PhysXManager::getInstance()->gPhysics, PhysXManager::getInstance()->gCooking);
	vehicleCreator.customizeVehicleToLengthScale(gLengthScale, gVehicle4W->getRigidDynamicActor(), &gVehicle4W->mWheelsSimData, &gVehicle4W->mDriveSimData);

	//  PxTransform startTransform(PxVec3(0,-10, 20), PxQuat(PxIdentity));


	startTransform = PxTransform(PxVec3(-330, -800, 1400), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gVehicle4W->getRigidDynamicActor()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	PhysXManager::getInstance()->gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);


	gravity = PhysXManager::getInstance()->gScene->getGravity();
	gVehicleInputData = new PxVehicleDrive4WRawInputData();

	gVehicleInputData->setDigitalAccel(false);
	gVehicleInputData->setDigitalSteerLeft(false);
	gVehicleInputData->setDigitalSteerRight(false);
	gVehicleInputData->setDigitalBrake(false);
	gVehicleInputData->setDigitalHandbrake(false);


	vehicleYoffset = vehicleTransform->drawPosition.y;
	vehicleXoffset = vehicleTransform->drawPosition.x;
	vehicleZoffset = vehicleTransform->drawPosition.z;

	//end init
}

VehicleDesc cVehicle::initVehicleDesc(cMesh* chassisMesh)
{

	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	chassisMesh->calcExtents();

	// glm::vec3 center = chassisMesh->maxExtents_XYZ - chassisMesh->minExtents_XYZ;
	// PxVec3 chassisDims(center.x, center.y, center.z);
	const PxVec3 chassisDims(4.5f, 1.0f, 5.0f);
	// const PxVec3 chassisDims(4.5f, 2.0f, 9.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) /** 0.8f*/ * chassisMass / 12,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12);
	//  const PxVec3 chassisMOI(1000, 800, 1000);
	  //fo
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f - 1.65f, -0.75f);
	//  const PxVec3 chassisCMOffset(0.0f, 0.0, 0.25f);

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

void cVehicle::SetChassis(TransformComponent* transformComponent)
{

	vehicleTransform = transformComponent;
}

void cVehicle::SetWheel(std::vector<TransformComponent*> wheels)
{

	this->wheels = wheels;
}



TransformComponent* cVehicle::GetChassis() const
{
	return vehicleTransform;
}

void cVehicle::Update(float deltaTime)
{
	//udpate start
	vehicleCreator.UpdateVehicle4W(deltaTime, gravity, gVehicle4W, NULL, gVehicleInputData);

	numShapes = gVehicle4W->getRigidDynamicActor()->getNbShapes();
	for (PxU32 i = 0; i < numShapes; i++)
	{
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
				glm::vec3 finalRotation = glm::vec3(rotation.x, rotation.y, rotation.z);
				wheels[carData->wheelId]->eulerRotation = finalRotation;
			}
			if (!carData->isWheel && carData->wheelId == -2)
			{
				// vehicleYoffset += pos.y;
				vehicleTransform->drawPosition = glm::vec3(pos.x, pos.y, pos.z);
				glm::vec3 finalRotation = glm::vec3(rotation.x + vehicleXrot, rotation.y, rotation.z);
				vehicleTransform->eulerRotation = finalRotation;
			}
		}
	}
	//update end


}

void cVehicle::reset()
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

void cVehicle::moveForward()
{
	//accelerate
	if (hasReversed)
	{
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		hasReversed = false;
	}
	gVehicleInputData->setDigitalAccel(true);
}

void cVehicle::moveBackward()
{
	//reverse
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	gVehicleInputData->setDigitalAccel(true);
	hasReversed = true;

}

void cVehicle::turnLeft()
{
	gVehicleInputData->setDigitalSteerLeft(false);
	gVehicleInputData->setDigitalSteerRight(true);
}

void cVehicle::turnRight()
{
	gVehicleInputData->setDigitalSteerLeft(true);
	gVehicleInputData->setDigitalSteerRight(false);
}

void cVehicle::stopTurning()
{
	gVehicleInputData->setDigitalSteerLeft(false);
	gVehicleInputData->setDigitalSteerRight(false);
}
