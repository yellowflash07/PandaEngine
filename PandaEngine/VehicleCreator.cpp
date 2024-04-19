#include "VehicleCreator.h"

//Angle thresholds used to categorize contacts as suspension contacts or rigid body contacts.
#define POINT_REJECT_ANGLE PxPi/4.0f
#define NORMAL_REJECT_ANGLE PxPi/4.0f

//Define the maximum acceleration for dynamic bodies under the wheel.
#define MAX_ACCELERATION 50.0f

PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);

	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}

PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eTOUCH);
}

PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking
(PxFilterData filterData0, PxFilterData filterData1,
	const void* constantBlock, PxU32 constantBlockSize,
	const PxQueryHit& hit)
{
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	if ((static_cast<const PxSweepHit&>(hit)).hadInitialOverlap())
		return PxQueryHitType::eNONE;
	return PxQueryHitType::eTOUCH;
}


VehicleCreator::VehicleCreator()
{
	PxInitVehicleSDK(*PhysXManager::getInstance()->gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);
	PxVehicleSetSweepHitRejectionAngles(POINT_REJECT_ANGLE, NORMAL_REJECT_ANGLE);
	PxVehicleSetMaxHitActorAcceleration(MAX_ACCELERATION);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 8, 1, WheelSceneQueryPreFilterNonBlocking, WheelSceneQueryPostFilterNonBlocking, PhysXManager::getInstance()->gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, PhysXManager::getInstance()->gScene);
	gFrictionPairs = createFrictionPairs(PhysXManager::getInstance()->gMaterial);
}

VehicleCreator::~VehicleCreator()
{
}

PxVehicleDrive4W* VehicleCreator::CreateVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxCooking* cooking)
{
	const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
	const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
	const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
	const PxU32 numWheels = vehicle4WDesc.numWheels;

	const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
	const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

	//Construct a physx actor with shapes for the chassis and wheels.
	//Set the rigid body mass, moment of inertia, and center of mass offset.
	PxRigidDynamic* veh4WActor = NULL;
	{
		//Construct a convex mesh for a cylindrical wheel.
		PxConvexMesh* wheelMesh = CreateWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
		//Assume all wheels are identical for simplicity.
		PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
		PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

		//Set the meshes and materials for the driven wheels.
		for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}
		//Set the meshes and materials for the non-driven wheels
		for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}

		//Chassis just has a single convex shape for simplicity.
		PxConvexMesh* chassisConvexMesh = CreateChassisMesh(chassisDims, *physics, *cooking);
		PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
		PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

		//Rigid body data.
		PxVehicleChassisData rigidBodyData;
		rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
		rigidBodyData.mMass = vehicle4WDesc.chassisMass;
		rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

		veh4WActor = CreateVehicleActor
		(rigidBodyData,
			wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
			chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
			*physics);
	}

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
	{
		//Compute the wheel center offsets from the origin.
		PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
		const PxF32 frontZ = chassisDims.z * 0.5f;	//HACK: was 0.3f
		const PxF32 rearZ = -chassisDims.z * 0.5f;
		ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims,
			wheelWidth, wheelRadius, numWheels, 
			wheelCenterActorOffsets);

		//Set up the simulation data for all wheels.
		setupWheelsSimulationData
		(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
			numWheels, wheelCenterActorOffsets,
			vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
			wheelsSimData);
	}

	//Set up the sim data for the vehicle drive model.
	PxVehicleDriveSimData4W driveSimData;
	{
		//Diff
		PxVehicleDifferential4WData diff;
		diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		driveSimData.setDiffData(diff);

		//Engine
		PxVehicleEngineData engine;
		engine.mPeakTorque = 500.0f;
		engine.mMaxOmega = 600.0f;//approx 6000 rpm
		driveSimData.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;
		gears.mSwitchTime = 0.2f;//HACK: was 0.5f
		driveSimData.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mStrength = 10.0f;
		clutch.mAccuracyMode = PxVehicleClutchAccuracyMode::eBEST_POSSIBLE;
		driveSimData.setClutchData(clutch);

		//Ackermann steer accuracy
		PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy = 1.0f;
		ackermann.mAxleSeparation =
			(wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z + vehicle4WDesc.axleWidth)-
			(wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z + vehicle4WDesc.axleWidth);
		ackermann.mFrontWidth =
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
		ackermann.mRearWidth =
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
		driveSimData.setAckermannGeometryData(ackermann);
	}

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
	vehDrive4W->setup(physics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

	configureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();
	this->vehDrive4W = vehDrive4W;
	return vehDrive4W;
}

PxVehicleKeySmoothingData gKeySmoothingData = { 
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);


void VehicleCreator::UpdateVehicle4W(const PxF32 timestep, const PxVec3& gravity, PxVehicleDrive4W* vehDrive4W, PxVehicleWheelQueryResult* vehWheelQueryResults, PxVehicleDrive4WRawInputData* vehDrive4WInputData)
{
	//Suspension sweeps (instead of raycasts).
	//Sweeps provide more information about the geometry under the wheel.
	PxVehicleWheels* vehicles[1] = { vehDrive4W };
	PxSweepQueryResult* sweepResults = gVehicleSceneQueryData->getSweepQueryResultBuffer(0);
	const PxU32 sweepResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionSweeps(gBatchQuery, 1, vehicles, sweepResultsSize, sweepResults, 8, NULL, 1.0f, 1.01f);

	//Vehicle update.
	const PxVec3 grav = PhysXManager::getInstance()->gScene->getGravity();
	PxVec3 scaledGrav = PxVec3(grav.x, grav.y, grav.z);
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS][1];
	PxVehicleWheelQueryResult vehicleQueryResults[1] =
	{
		{ wheelQueryResults[0], vehDrive4W->mWheelsSimData.getNbWheels() },
	};
	PxVehicleUpdates(timestep, scaledGrav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//update input
	PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, *vehDrive4WInputData, timestep, false, *vehDrive4W);

}

void VehicleCreator::customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData4W* driveSimData)
{
	customizeVehicleToLengthScale(lengthScale, rigidDynamic, wheelsSimData, static_cast<PxVehicleDriveSimData*>(driveSimData));

	//Ackermann geometry.
	if (driveSimData)
	{
		PxVehicleAckermannGeometryData ackermannData = driveSimData->getAckermannGeometryData();
		ackermannData.mAxleSeparation *= lengthScale;
		ackermannData.mFrontWidth *= lengthScale;
		ackermannData.mRearWidth *= lengthScale;
		driveSimData->setAckermannGeometryData(ackermannData);
	}

	for (PxU32 i = 0; i < gSteerVsForwardSpeedTable.mNbDataPairs; i++)
	{
		gSteerVsForwardSpeedTable.mDataPairs[2 * i + 0] *= lengthScale;
	}
}

void VehicleCreator::customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData* driveSimData)
{
	{
		//Rigid body center of mass and moment of inertia.
		{
			PxTransform t = rigidDynamic->getCMassLocalPose();
			t.p *= lengthScale;
			rigidDynamic->setCMassLocalPose(t);

			PxVec3 moi = rigidDynamic->getMassSpaceInertiaTensor();
			moi *= (lengthScale * lengthScale);
			rigidDynamic->setMassSpaceInertiaTensor(moi);
		}

		//Wheels, suspensions, wheel centers, tire/susp force application points.
		{
			for (PxU32 i = 0; i < wheelsSimData->getNbWheels(); i++)
			{
				PxVehicleWheelData wheelData = wheelsSimData->getWheelData(i);
				wheelData.mRadius *= lengthScale;
				wheelData.mWidth *= lengthScale;
				wheelData.mDampingRate *= lengthScale * lengthScale;
				wheelData.mMaxBrakeTorque *= lengthScale * lengthScale;
				wheelData.mMaxHandBrakeTorque *= lengthScale * lengthScale;
				wheelData.mMOI *= lengthScale * lengthScale;
				wheelsSimData->setWheelData(i, wheelData);

				PxVehicleSuspensionData suspData = wheelsSimData->getSuspensionData(i);
				suspData.mMaxCompression *= lengthScale;
				suspData.mMaxDroop *= lengthScale;
				wheelsSimData->setSuspensionData(i, suspData);

				PxVec3 v = wheelsSimData->getWheelCentreOffset(i);
				v *= lengthScale;
				wheelsSimData->setWheelCentreOffset(i, v);

				v = wheelsSimData->getSuspForceAppPointOffset(i);
				v *= lengthScale;
				wheelsSimData->setSuspForceAppPointOffset(i, v);

				v = wheelsSimData->getTireForceAppPointOffset(i);
				v *= lengthScale;
				wheelsSimData->setTireForceAppPointOffset(i, v);
			}
		}

		//Slow forward speed correction.
		{
			wheelsSimData->setSubStepCount(5.0f * lengthScale, 3, 1);
			wheelsSimData->setMinLongSlipDenominator(4.0f * lengthScale);
		}

		//Engine
		if (driveSimData)
		{
			PxVehicleEngineData engineData = driveSimData->getEngineData();
			engineData.mMOI *= lengthScale * lengthScale;
			engineData.mPeakTorque *= lengthScale * lengthScale;
			engineData.mDampingRateFullThrottle *= lengthScale * lengthScale;
			engineData.mDampingRateZeroThrottleClutchEngaged *= lengthScale * lengthScale;
			engineData.mDampingRateZeroThrottleClutchDisengaged *= lengthScale * lengthScale;
			driveSimData->setEngineData(engineData);
		}

		//Clutch.
		if (driveSimData)
		{
			PxVehicleClutchData clutchData = driveSimData->getClutchData();
			clutchData.mStrength *= lengthScale * lengthScale;
			driveSimData->setClutchData(clutchData);
		}

		//Scale the collision meshes too.
		{
			PxShape* shapes[16];
			const PxU32 nbShapes = rigidDynamic->getShapes(shapes, 16);
			for (PxU32 i = 0; i < nbShapes; i++)
			{
				switch (shapes[i]->getGeometryType())
				{
				case PxGeometryType::eSPHERE:
				{
					PxSphereGeometry sphere;
					shapes[i]->getSphereGeometry(sphere);
					sphere.radius *= lengthScale;
					shapes[i]->setGeometry(sphere);
				}
				break;
				case PxGeometryType::ePLANE:
					PX_ASSERT(false);
					break;
				case PxGeometryType::eCAPSULE:
				{
					PxCapsuleGeometry capsule;
					shapes[i]->getCapsuleGeometry(capsule);
					capsule.radius *= lengthScale;
					capsule.halfHeight *= lengthScale;
					shapes[i]->setGeometry(capsule);
				}
				break;
				case PxGeometryType::eBOX:
				{
					PxBoxGeometry box;
					shapes[i]->getBoxGeometry(box);
					box.halfExtents *= lengthScale;
					shapes[i]->setGeometry(box);
				}
				break;
				case PxGeometryType::eCONVEXMESH:
				{
					PxConvexMeshGeometry convexMesh;
					shapes[i]->getConvexMeshGeometry(convexMesh);
					convexMesh.scale.scale *= lengthScale;
					shapes[i]->setGeometry(convexMesh);
				}
				break;
				case PxGeometryType::eTRIANGLEMESH:
				{
					PxTriangleMeshGeometry triMesh;
					shapes[i]->getTriangleMeshGeometry(triMesh);
					triMesh.scale.scale *= lengthScale;
					shapes[i]->setGeometry(triMesh);
				}
				break;
				case PxGeometryType::eHEIGHTFIELD:
				{
					PxHeightFieldGeometry hf;
					shapes[i]->getHeightFieldGeometry(hf);
					hf.columnScale *= lengthScale;
					hf.heightScale *= lengthScale;
					hf.rowScale *= lengthScale;
					shapes[i]->setGeometry(hf);
				}
				break;
				case PxGeometryType::eINVALID:
				case PxGeometryType::eGEOMETRY_COUNT:
					break;
				}
			}
		}
	}
}

PxRigidDynamic* VehicleCreator::CreateVehicleActor(const PxVehicleChassisData& chassisData, PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, const PxFilterData& wheelSimFilterData, PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, const PxFilterData& chassisSimFilterData, PxPhysics& physics)
{
	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	setupNonDrivableSurface(wheelQryFilterData);
	PxFilterData chassisQryFilterData;
	setupNonDrivableSurface(chassisQryFilterData);

	PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(PxIdentity));
	//Add all the wheel shapes to the actor.
	for (PxU32 i = 0; i < numWheels; i++)
	{
		PxConvexMeshGeometry geom(wheelConvexMeshes[i]);

		PxShape* wheelShape = PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);

		wheelShape->setQueryFilterData(wheelQryFilterData);
		wheelShape->setSimulationFilterData(wheelSimFilterData);
		wheelShape->setLocalPose(PxTransform(PxIdentity));

		ShapeUserData* chassisData = new ShapeUserData();
		chassisData->isWheel = true;
		chassisData->wheelId = i;

		wheelShape->userData = chassisData;


		PxTransform shapePose = PxShapeExt::getGlobalPose(*wheelShape, *vehActor);
	}

	//Add the chassis shapes to the actor.
	for (PxU32 i = 0; i < numChassisMeshes; i++)
	{
		PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
		chassisShape->setQueryFilterData(chassisQryFilterData);
		chassisShape->setSimulationFilterData(chassisSimFilterData);
		chassisShape->setLocalPose(PxTransform(PxIdentity));

		ShapeUserData* chassisData = new ShapeUserData();
		chassisData->isWheel = false;
		chassisData->wheelId = -2;

		chassisShape->userData = chassisData;
		/*CarData* chassisData = new CarData();
		chassisData->carPart = CHASSIS;
		chassisData->index = i;
		chassisData->offset = glm::vec3(chassisShape->getLocalPose().p.x, chassisShape->getLocalPose().p.y, chassisShape->getLocalPose().p.z);
		chassisShape->userData = chassisData;*/
	}

	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

	return vehActor;
}

PxConvexMesh* VehicleCreator::CreateWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
{
	{
		PxVec3 points[2 * 16];
		for (PxU32 i = 0; i < 16; i++)
		{
			const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 16.0f);
			const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 16.0f);
			const PxF32 y = radius * cosTheta;
			const PxF32 z = radius * sinTheta;
			points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
			points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
		}

		return createConvexMesh(points, 32, physics, cooking);
	}
}

PxConvexMesh* VehicleCreator::CreateChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{

	const PxF32 x = dims.x * 0.5f;
	const PxF32 y = dims.y * 0.5f;
	const PxF32 z = dims.z * 0.5f;
	PxVec3 verts[8] =
	{
		PxVec3(x,y,-z),
		PxVec3(x,y,z),
		PxVec3(x,-y,z),
		PxVec3(x,-y,-z),
		PxVec3(-x,y,-z),
		PxVec3(-x,y,z),
		PxVec3(-x,-y,z),
		PxVec3(-x,-y,-z)
	};

	return createConvexMesh(verts, 8, physics, cooking);

}

PxConvexMesh* VehicleCreator::createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
{
	{
		// Create descriptor for convex mesh
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxConvexMesh* convexMesh = NULL;
		PxDefaultMemoryOutputStream buf;
		if (cooking.cookConvexMesh(convexDesc, buf))
		{
			PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = physics.createConvexMesh(id);
		}

		return convexMesh;
	}
}

void VehicleCreator::ComputeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, 
	const PxF32 wheelRearZ, const PxVec3& chassisDims,
	const PxF32 wheelWidth, const PxF32 wheelRadius, 
	const PxU32 numWheels, PxVec3* wheelCentreOffsets)
{

	//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
	//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
	//Compute a position for the front wheel and the rear wheel along the z-axis.
	//Compute the separation between each wheel along the z-axis.
	const PxF32 numLeftWheels = numWheels / 2.0f;
	PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f); 
	//Set the outside of the left and right wheels to be flush with the chassis.
	//Set the top of the wheel to be just touching the underside of the chassis.
	//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
	//HACK:
	float Xoffset =0.75f;
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3(((-chassisDims.x + wheelWidth) * 0.5f) - Xoffset,
																		-(chassisDims.y / 2 + wheelRadius), 
																		wheelRearZ + 0 * deltaZ * 0.5f);


	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3(((+chassisDims.x - wheelWidth) * 0.5f) + Xoffset, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3(((-chassisDims.x + wheelWidth) * 0.5f) - Xoffset, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3(((+chassisDims.x - wheelWidth) * 0.5f) + Xoffset, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);

}

void VehicleCreator::setupWheelsSimulationData(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth, const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets, const PxVec3& chassisCMOffset, const PxF32 chassisMass, PxVehicleWheelsSimData* wheelsSimData)
{
	{
		//Set up the wheels.
		PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
		{
			//Set up the wheel data structures with mass, moi, radius, width.
			for (PxU32 i = 0; i < numWheels; i++)
			{
				wheels[i].mMass = wheelMass;
				wheels[i].mMOI = wheelMOI;
				wheels[i].mRadius = wheelRadius;
				wheels[i].mWidth = wheelWidth;
			}

			//Enable the handbrake for the rear wheels only.
			wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
			wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
			//Enable steering for the front wheels only.
			wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.3333f;
			wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.3333f;
		}

		//Set up the tires.
		PxVehicleTireData tires[PX_MAX_NB_WHEELS];
		{
			//Set up the tires.
			for (PxU32 i = 0; i < numWheels; i++)
			{
				tires[i].mType = 0;
				//tires[i].
			}
		}

		//Set up the suspensions
		PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
		{
			//Compute the mass supported by each suspension spring.
			PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
			PxVehicleComputeSprungMasses
			(numWheels, wheelCenterActorOffsets,
				chassisCMOffset, chassisMass, 1, suspSprungMasses);

			//Set the suspension data.
			for (PxU32 i = 0; i < numWheels; i++)
			{
				suspensions[i].mMaxCompression = 0.01f;
				suspensions[i].mMaxDroop = 0.1f;
				suspensions[i].mSpringStrength = 35000.0f;
				suspensions[i].mSpringDamperRate = 6000.0f;
				suspensions[i].mSprungMass = suspSprungMasses[i];
			}

			//Set the camber angles.
			const PxF32 camberAngleAtRest = 0.0;
			const PxF32 camberAngleAtMaxDroop = 0.01f;
			const PxF32 camberAngleAtMaxCompression = -0.01f;
			for (PxU32 i = 0; i < numWheels; i += 2)
			{
				suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
				suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
				suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
				suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
				suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
				suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
			}
		}

		//Set up the wheel geometry.
		PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
		PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
		PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
		PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
		{
			//Set the geometry data.
			for (PxU32 i = 0; i < numWheels; i++)
			{
				//Vertical suspension travel.
				suspTravelDirections[i] = PxVec3(0, -1, 0);

				//Wheel center offset is offset from rigid body center of mass.
				wheelCentreCMOffsets[i] =
					wheelCenterActorOffsets[i] - chassisCMOffset;

				//Suspension force application point 0.3 metres below 
				//rigid body center of mass.
				suspForceAppCMOffsets[i] =
					PxVec3(wheelCentreCMOffsets[i].x, -0.1f, wheelCentreCMOffsets[i].z);

				//Tire force application point 0.3 metres below 
				//rigid body center of mass.
				tireForceAppCMOffsets[i] =
					PxVec3(wheelCentreCMOffsets[i].x, -0.1f, wheelCentreCMOffsets[i].z);
			}
		}

		//Set up the filter data of the raycast that will be issued by each suspension.
		PxFilterData qryFilterData;
		setupNonDrivableSurface(qryFilterData);

		//Set the wheel, tire and suspension data.
		//Set the geometry data.
		//Set the query filter data
		for (PxU32 i = 0; i < numWheels; i++)
		{
			wheelsSimData->setWheelData(i, wheels[i]);
			wheelsSimData->setTireData(i, tires[i]);
			wheelsSimData->setSuspensionData(i, suspensions[i]);
			wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
			wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
			wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
			wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
			wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
			wheelsSimData->setWheelShapeMapping(i, PxI32(i));
		}

		wheelsSimData->setSubStepCount(5.0f, 3, 1);

		//Add a front and rear anti-roll bar
		PxVehicleAntiRollBarData barFront;
		barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
		barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
		barFront.mStiffness = 4000.0f;
		wheelsSimData->addAntiRollBarData(barFront);
		PxVehicleAntiRollBarData barRear;
		barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
		barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
		barRear.mStiffness = 4000.0f;
		wheelsSimData->addAntiRollBarData(barRear);
	}
}

void VehicleCreator::setupNonDrivableSurface(PxFilterData filterData)
{
	filterData.word3 = UNDRIVABLE_SURFACE;
}

void VehicleCreator::setupDrivableSurface(PxFilterData wheelQryFilterData)
{
	wheelQryFilterData.word3 = DRIVABLE_SURFACE;
}

//Tire model friction for each combination of drivable surface type and tire type.
static PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
	//NORMAL,	WORN
	{1.00f,		0.1f}//TARMAC
};

PxVehicleDrivableSurfaceToTireFrictionPairs* VehicleCreator::createFrictionPairs(const PxMaterial* defaultMaterial)
{
	PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

	const PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

	for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}

void VehicleCreator::configureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas)
{
	if (actorUserData)
	{
		vehicle->getRigidDynamicActor()->userData = actorUserData;
		actorUserData->vehicle = vehicle;
	}

	/*if (shapeUserDatas)
	{
		PxShape* shapes[PX_MAX_NB_WHEELS + 1];
		vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
		for (PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
		{
			const PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
			shapes[shapeId]->userData = &shapeUserDatas[i];
			shapeUserDatas[i].isWheel = true;
			shapeUserDatas[i].wheelId = i;
		}
	}*/
}
