#pragma once

#include "PhysXManager.h"
enum
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};
enum
{
	COLLISION_FLAG_GROUND = 1 << 0,
	COLLISION_FLAG_WHEEL = 1 << 1,
	COLLISION_FLAG_CHASSIS = 1 << 2,
	COLLISION_FLAG_OBSTACLE = 1 << 3,
	COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

	COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};





class VehicleSceneQueryData
{
public:
	VehicleSceneQueryData();
	~VehicleSceneQueryData();

	//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
	static VehicleSceneQueryData* allocate
	(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 maxNumHitPointsPerWheel, const PxU32 numVehiclesInBatch,
		PxBatchQueryPreFilterShader preFilterShader, PxBatchQueryPostFilterShader postFilterShader,
		PxAllocatorCallback& allocator);

	//Free allocated buffers.
	void free(PxAllocatorCallback& allocator);

	//Create a PxBatchQuery instance that will be used for a single specified batch.
	static PxBatchQuery* setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene);

	//Return an array of scene query results for a single specified batch.
	PxRaycastQueryResult* getRaycastQueryResultBuffer(const PxU32 batchId);

	//Return an array of scene query results for a single specified batch.
	PxSweepQueryResult* getSweepQueryResultBuffer(const PxU32 batchId);

	//Get the number of scene query results that have been allocated for a single batch.
	PxU32 getQueryResultBufferSize() const;



private:

	//Number of queries per batch
	PxU32 mNumQueriesPerBatch;

	//Number of hit results per query
	PxU32 mNumHitResultsPerQuery;

	//One result for each wheel.
	PxRaycastQueryResult* mRaycastResults;
	PxSweepQueryResult* mSweepResults;

	//One hit for each wheel.
	PxRaycastHit* mRaycastHitBuffer;
	PxSweepHit* mSweepHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	PxBatchQueryPreFilterShader mPreFilterShader;

	//Filter shader used to reject hit shapes that initially overlap sweeps.
	PxBatchQueryPostFilterShader mPostFilterShader;
};

