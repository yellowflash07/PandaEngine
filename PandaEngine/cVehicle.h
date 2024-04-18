#pragma once
#include "VehicleCreator.h"
#include "Engine.h"

class cVehicle
{
public:
	cVehicle();
	~cVehicle();

	void vehicleInit();

	VehicleDesc initVehicleDesc(cMesh* chassisMesh);
	void SetChassis(TransformComponent* transformComponent);
	void SetWheel(std::vector<TransformComponent*> wheels);
 
	TransformComponent* GetChassis() const;
	PxTransform startTransform;

	void Update(float deltaTime);

	//Controls
	void reset();
	void moveForward();
	void moveBackward();
	void turnLeft();
	void turnRight();
	void stopTurning();
	void brake();
	glm::vec3 startPos;

private:

	TransformComponent* vehicleTransform;
	cMesh* vehicleMesh;


	//vehicle.chassisTransformComponent = 4
	std::vector<TransformComponent*> wheels;
	//GameObject* wheel;

	//init VehicleDesc

	const PxF32 chassisMass = 1500.0f;
	VehicleDesc vehicleDesc;

	const PxVec3 chassisMOI;
	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	 //Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 1.38f;
	//   const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.85f;
	// const PxF32 wheelWidth =  0.4f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;



	//Init
	VehicleCreator vehicleCreator;
	PxVehicleDrive4W* gVehicle4W;
	float gLengthScale = 18.0f;
	PxVec3 gravity;
	PxVehicleDrive4WRawInputData* gVehicleInputData;

	float vehicleYoffset;
	float vehicleXoffset;
	float vehicleZoffset;

	//Update
	float deltaTime;
	int numShapes;
	PxShape* shape;
	float vehicleXrot = -1.6f;

	//Movements Controls
	bool hasReversed = false;

};

