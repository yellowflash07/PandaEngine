#pragma once
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/pvd/PxPvd.h>
#include "TransformComponent.h"
#include "PhysXManager.h"
#include "IEditorUI.h"
using namespace physx;

enum ColliderType
{
	BOX,
	SPHERE,
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
private:
	PxShape* shape;
	PxRigidActor* body;
	PxMaterial* material;
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	TransformComponent* transform;
	ColliderType type;	
	bool isDynamic;
};

