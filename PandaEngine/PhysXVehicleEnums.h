#pragma once
enum SURFACE_FLAG
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};
enum COLLISION_FLAG
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


//Drivable surface types.
enum
{
	SURFACE_TYPE_TARMAC,
	MAX_NUM_SURFACE_TYPES
};
//Tire types.
enum
{
	TIRE_TYPE_NORMAL = 0,
	TIRE_TYPE_WORN,
	MAX_NUM_TIRE_TYPES
};

struct ActorUserData
{
	ActorUserData()
		: vehicle(NULL),
		actor(NULL)
	{
	}

	const PxVehicleWheels* vehicle;
	const PxActor* actor;
};

struct ShapeUserData
{
	ShapeUserData()
		: isWheel(false),
		wheelId(0xffffffff)
	{
	}

	bool isWheel;
	PxU32 wheelId;
};
