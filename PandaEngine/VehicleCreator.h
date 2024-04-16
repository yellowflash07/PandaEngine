#pragma once
#include "PhysXManager.h"
#include "VehicleSceneQueryData.h"

//enum CarPart
//{
//	WHEEL,
//	CHASSIS
//};

//struct CarData
//{
//	CarPart carPart;
//	int index;
//	glm::vec3 offset;
//};

struct VehicleDesc
{


	VehicleDesc()
		: chassisMass(0.0f),
		chassisDims(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisMOI(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisCMOffset(PxVec3(0.0f, 0.0f, 0.0f)),
		chassisMaterial(NULL),
		wheelMass(0.0f),
		wheelWidth(0.0f),
		wheelRadius(0.0f),
		wheelMOI(0.0f),
		wheelMaterial(NULL)
	{
	}

	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxF32 axleWidth;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
	PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

	ActorUserData* actorUserData;
	ShapeUserData* shapeUserDatas;
};

class VehicleCreator
{
public:


	VehicleCreator();
	~VehicleCreator();

	PxVehicleDrive4W* CreateVehicle4W(const VehicleDesc& vehDesc, PxPhysics* physics, PxCooking* cooking);

	void UpdateVehicle4W(const PxF32 timestep, const PxVec3& gravity, PxVehicleDrive4W* vehDrive4W, PxVehicleWheelQueryResult* vehWheelQueryResults, PxVehicleDrive4WRawInputData* vehDrive4WInputData);

	PxVehicleDrive4W* vehDrive4W;

	void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData4W* driveSimData);

private:
	void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData* driveSimData);

	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;
	PxRigidDynamic* CreateVehicleActor
	(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, const PxFilterData& wheelSimFilterData,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, const PxFilterData& chassisSimFilterData,
		PxPhysics& physics);

	PxConvexMesh* CreateWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* CreateChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);
	void ComputeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets);
	void setupWheelsSimulationData
	(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);

	void setupNonDrivableSurface(PxFilterData wheelQryFilterData);
	void setupDrivableSurface(PxFilterData wheelQryFilterData);
	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial);

	void configureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas);
};

