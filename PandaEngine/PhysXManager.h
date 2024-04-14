#pragma once

#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/pvd/PxPvd.h>
#include "TransformComponent.h"
#include <PhysX/characterkinematic/PxControllerManager.h>
#include <map>
#include "../cVAOManager/sModelDrawInfo.h"
#include "DebugRenderer.h"

using namespace physx;
#define PVD_HOST "127.0.0.1"

class PhysXManager
{
public:
	PhysXManager();
	~PhysXManager();

	static PhysXManager* getInstance();

	void Init(bool connectToPvd);
	void Update(float deltaTime);
	void DebugUpdate(float deltaTime);
	void DrawDebug();
	void Shutdown();
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	PxCooking* gCooking = NULL;
	PxDefaultAllocator		gAllocator;
	bool updateDebug;
private:

	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;

	bool initialized = false;

	PxPvd* gPvd = NULL;

	int lastActorCount = 0;
};


//physx::PxTransform ConvertMat4ToPxTransform(const glm::mat4& matrix)
//{
//    glm::vec3 translation = glm::vec3(matrix[3]); // Extract translation component
//
//    // Extract rotation component
//    glm::mat3 rotationMatrix = glm::mat3(matrix);
//    physx::PxQuat rotation;
//    rotation.x = rotationMatrix[0][0];
//    rotation.y = rotationMatrix[1][0];
//    rotation.z = rotationMatrix[2][0];
//    rotation.w = 1.0f + rotationMatrix[0][0] + rotationMatrix[1][1] + rotationMatrix[2][2];
//    rotation.w = sqrtf(rotation.w) * 0.5f;
//
//    // Create PxTransform
//    physx::PxTransform transform;
//    transform.p = physx::PxVec3(translation.x, translation.y, translation.z);
//    transform.q = rotation;
//
//    return transform;
//}