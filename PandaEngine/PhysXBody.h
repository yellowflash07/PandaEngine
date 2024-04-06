#pragma once
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/pvd/PxPvd.h>
#include "TransformComponent.h"
#include "PhysXManager.h"
#include "cMesh.h"
#include "IEditorUI.h"
using namespace physx;

enum ColliderType
{
	NONE,
	BOX,
	SPHERE,
	MESH
};


class PhysXBody : public IEditorUI
{
public:
	PhysXBody(TransformComponent* transform);
	~PhysXBody();

	void SetShape(ColliderType type);
	void SetBody(bool isDynamic);
	void Update(float deltaTime);
	void Render() override;
	ColliderType type;
	PxShape* shape;
	bool isDynamic;
	void CreateMeshCollider(cMesh* mesh,
		bool skipMeshCleanup, bool skipEdgeData, bool inserted, const PxU32 numTrisPerLeaf);
private:
	PxRigidActor* body;
	PxMaterial* material;
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	TransformComponent* transform;
	bool setMesh = false;
	bool isTrigger = false;
	void SetupCommonCookingParams(PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData);
	void SetTrigger();
};

